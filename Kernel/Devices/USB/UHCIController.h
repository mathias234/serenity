/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2020-2021, Jesse Buhagiar <jooster669@gmail.com>
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

#pragma once

#include <AK/Platform.h>

#include <AK/NonnullOwnPtr.h>
#include <Kernel/Devices/USB/UHCIDescriptorTypes.h>
#include <Kernel/IO.h>
#include <Kernel/PCI/Device.h>
#include <Kernel/Process.h>
#include <Kernel/Time/TimeManagement.h>
#include <Kernel/VM/ContiguousVMObject.h>

namespace Kernel::USB {

class UHCIController final : public PCI::Device {

public:
    static void detect();
    static UHCIController& the();
    virtual ~UHCIController() override;

    void reset();
    void stop();
    void start();
    void spawn_port_proc();

    void do_debug_transfer();

private:
    UHCIController(PCI::Address, PCI::ID);

    u16 read_usbcmd() { return m_io_base.offset(0).in<u16>(); }
    u16 read_usbsts() { return m_io_base.offset(0x2).in<u16>(); }
    u16 read_usbintr() { return m_io_base.offset(0x4).in<u16>(); }
    u16 read_frnum() { return m_io_base.offset(0x6).in<u16>(); }
    u32 read_flbaseadd() { return m_io_base.offset(0x8).in<u32>(); }
    u8 read_sofmod() { return m_io_base.offset(0xc).in<u8>(); }
    u16 read_portsc1() { return m_io_base.offset(0x10).in<u16>(); }
    u16 read_portsc2() { return m_io_base.offset(0x12).in<u16>(); }

    void write_usbcmd(u16 value) { m_io_base.offset(0).out(value); }
    void write_usbsts(u16 value) { m_io_base.offset(0x2).out(value); }
    void write_usbintr(u16 value) { m_io_base.offset(0x4).out(value); }
    void write_frnum(u16 value) { m_io_base.offset(0x6).out(value); }
    void write_flbaseadd(u32 value) { m_io_base.offset(0x8).out(value); }
    void write_sofmod(u8 value) { m_io_base.offset(0xc).out(value); }
    void write_portsc1(u16 value) { m_io_base.offset(0x10).out(value); }
    void write_portsc2(u16 value) { m_io_base.offset(0x12).out(value); }

    virtual void handle_irq(const RegisterState&) override;

    void create_structures();
    void setup_schedule();

    QueueHead* allocate_queue_head() const;
    TransferDescriptor* allocate_transfer_descriptor() const;

private:
    IOAddress m_io_base;

    Vector<QueueHead*> m_free_qh_pool;
    Vector<TransferDescriptor*> m_free_td_pool;
    Vector<TransferDescriptor*> m_iso_td_list;

    QueueHead* m_interrupt_transfer_queue;
    QueueHead* m_lowspeed_control_qh;
    QueueHead* m_fullspeed_control_qh;
    QueueHead* m_bulk_qh;
    QueueHead* m_dummy_qh; // Needed for PIIX4 hack

    OwnPtr<Region> m_framelist;
    OwnPtr<Region> m_qh_pool;
    OwnPtr<Region> m_td_pool;
    OwnPtr<Region> m_td_buffer_region;
};

}
