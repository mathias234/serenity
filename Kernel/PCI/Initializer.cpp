/*
 * Copyright (c) 2020, Liav A. <liavalb@hotmail.co.il>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Kernel/ACPI/Parser.h>
#include <Kernel/CommandLine.h>
#include <Kernel/IO.h>
#include <Kernel/PCI/IOAccess.h>
#include <Kernel/PCI/Initializer.h>
#include <Kernel/PCI/MMIOAccess.h>
#include <Kernel/PCI/WindowedMMIOAccess.h>
#include <Kernel/Panic.h>

namespace Kernel {
namespace PCI {

static bool test_pci_io();

UNMAP_AFTER_INIT static PCIAccessLevel detect_optimal_access_type(PCIAccessLevel boot_determined)
{
    if (!ACPI::is_enabled() || ACPI::Parser::the()->find_table("MCFG").is_null())
        return PCIAccessLevel::IOAddressing;

    if (boot_determined != PCIAccessLevel::IOAddressing)
        return boot_determined;

    if (test_pci_io())
        return PCIAccessLevel::IOAddressing;

    PANIC("No PCI bus access method detected!");
}

UNMAP_AFTER_INIT void initialize()
{
    auto boot_determined = kernel_command_line().pci_access_level();

    switch (detect_optimal_access_type(boot_determined)) {
    case PCIAccessLevel::MappingPerDevice:
        WindowedMMIOAccess::initialize(ACPI::Parser::the()->find_table("MCFG"));
        break;
    case PCIAccessLevel::MappingPerBus:
        MMIOAccess::initialize(ACPI::Parser::the()->find_table("MCFG"));
        break;
    case PCIAccessLevel::IOAddressing:
        IOAccess::initialize();
        break;
    default:
        VERIFY_NOT_REACHED();
    }

    PCI::enumerate([&](const Address& address, ID id) {
        dmesgln("{} {}", address, id);
    });
}

UNMAP_AFTER_INIT bool test_pci_io()
{
    dmesgln("Testing PCI via manual probing...");
    u32 tmp = 0x80000000;
    IO::out32(PCI_ADDRESS_PORT, tmp);
    tmp = IO::in32(PCI_ADDRESS_PORT);
    if (tmp == 0x80000000) {
        dmesgln("PCI IO supported");
        return true;
    }

    dmesgln("PCI IO not supported");
    return false;
}

}
}
