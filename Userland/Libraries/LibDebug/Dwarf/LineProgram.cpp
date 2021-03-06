/*
 * Copyright (c) 2020, Itamar S. <itamar8910@gmail.com>
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

#include "LineProgram.h"
#include <AK/Debug.h>
#include <AK/String.h>
#include <AK/StringBuilder.h>

namespace Debug::Dwarf {

LineProgram::LineProgram(InputMemoryStream& stream)
    : m_stream(stream)
{
    m_unit_offset = m_stream.offset();
    parse_unit_header();
    parse_source_directories();
    parse_source_files();
    run_program();
}

void LineProgram::parse_unit_header()
{
    m_stream >> Bytes { &m_unit_header, sizeof(m_unit_header) };

    VERIFY(m_unit_header.version == DWARF_VERSION);
    VERIFY(m_unit_header.opcode_base == SPECIAL_OPCODES_BASE);

    dbgln_if(DWARF_DEBUG, "unit length: {}", m_unit_header.length);
}

void LineProgram::parse_source_directories()
{
    m_source_directories.append(".");

    while (m_stream.peek_or_error()) {
        String directory;
        m_stream >> directory;
        dbgln_if(DWARF_DEBUG, "directory: {}", directory);
        m_source_directories.append(move(directory));
    }
    m_stream.handle_recoverable_error();
    m_stream.discard_or_error(1);
    VERIFY(!m_stream.has_any_error());
}

void LineProgram::parse_source_files()
{
    m_source_files.append({ ".", 0 });
    while (!m_stream.eof() && m_stream.peek_or_error()) {
        String file_name;
        m_stream >> file_name;
        size_t directory_index = 0;
        m_stream.read_LEB128_unsigned(directory_index);
        size_t _unused = 0;
        m_stream.read_LEB128_unsigned(_unused); // skip modification time
        m_stream.read_LEB128_unsigned(_unused); // skip file size
        dbgln_if(DWARF_DEBUG, "file: {}, directory index: {}", file_name, directory_index);
        m_source_files.append({ file_name, directory_index });
    }
    m_stream.discard_or_error(1);
    VERIFY(!m_stream.has_any_error());
}

void LineProgram::append_to_line_info()
{
    dbgln_if(DWARF_DEBUG, "appending line info: {:p}, {}:{}", m_address, m_source_files[m_file_index].name, m_line);
    if (!m_is_statement)
        return;

    if (m_file_index >= m_source_files.size())
        return;

    String directory = m_source_directories[m_source_files[m_file_index].directory_index];

    StringBuilder full_path(directory.length() + m_source_files[m_file_index].name.length() + 1);
    full_path.append(directory);
    full_path.append('/');
    full_path.append(m_source_files[m_file_index].name);

    m_lines.append({ m_address, full_path.to_string(), m_line });
}

void LineProgram::reset_registers()
{
    m_address = 0;
    m_line = 1;
    m_file_index = 1;
    m_is_statement = m_unit_header.default_is_stmt == 1;
}

void LineProgram::handle_extended_opcode()
{
    size_t length = 0;
    m_stream.read_LEB128_unsigned(length);

    u8 sub_opcode = 0;
    m_stream >> sub_opcode;

    switch (sub_opcode) {
    case ExtendedOpcodes::EndSequence: {
        append_to_line_info();
        reset_registers();
        break;
    }
    case ExtendedOpcodes::SetAddress: {
        VERIFY(length == sizeof(size_t) + 1);
        m_stream >> m_address;
        dbgln_if(DWARF_DEBUG, "SetAddress: {:p}", m_address);
        break;
    }
    case ExtendedOpcodes::SetDiscriminator: {
        dbgln_if(DWARF_DEBUG, "SetDiscriminator");
        m_stream.discard_or_error(1);
        break;
    }
    default:
        dbgln_if(DWARF_DEBUG, "offset: {:p}", m_stream.offset());
        VERIFY_NOT_REACHED();
    }
}
void LineProgram::handle_standard_opcode(u8 opcode)
{
    switch (opcode) {
    case StandardOpcodes::Copy: {
        append_to_line_info();
        break;
    }
    case StandardOpcodes::AdvancePc: {
        size_t operand = 0;
        m_stream.read_LEB128_unsigned(operand);
        size_t delta = operand * m_unit_header.min_instruction_length;
        dbgln_if(DWARF_DEBUG, "AdvancePC by: {} to: {:p}", delta, m_address + delta);
        m_address += delta;
        break;
    }
    case StandardOpcodes::SetFile: {
        size_t new_file_index = 0;
        m_stream.read_LEB128_unsigned(new_file_index);
        dbgln_if(DWARF_DEBUG, "SetFile: new file index: {}", new_file_index);
        m_file_index = new_file_index;
        break;
    }
    case StandardOpcodes::SetColumn: {
        // not implemented
        dbgln_if(DWARF_DEBUG, "SetColumn");
        size_t new_column;
        m_stream.read_LEB128_unsigned(new_column);

        break;
    }
    case StandardOpcodes::AdvanceLine: {
        ssize_t line_delta;
        m_stream.read_LEB128_signed(line_delta);
        VERIFY(line_delta >= 0 || m_line >= (size_t)(-line_delta));
        m_line += line_delta;
        dbgln_if(DWARF_DEBUG, "AdvanceLine: {}", m_line);
        break;
    }
    case StandardOpcodes::NegateStatement: {
        dbgln_if(DWARF_DEBUG, "NegateStatement");
        m_is_statement = !m_is_statement;
        break;
    }
    case StandardOpcodes::ConstAddPc: {
        u8 adjusted_opcode = 255 - SPECIAL_OPCODES_BASE;
        ssize_t address_increment = (adjusted_opcode / m_unit_header.line_range) * m_unit_header.min_instruction_length;
        address_increment *= m_unit_header.min_instruction_length;
        dbgln_if(DWARF_DEBUG, "ConstAddPc: advance pc by: {} to: {}", address_increment, (m_address + address_increment));
        m_address += address_increment;
        break;
    }
    case StandardOpcodes::SetIsa: {
        size_t isa;
        m_stream.read_LEB128_unsigned(isa);
        dbgln_if(DWARF_DEBUG, "SetIsa: {}", isa);
        break;
    }
    case StandardOpcodes::FixAdvancePc: {
        u16 delta = 0;
        m_stream >> delta;
        dbgln_if(DWARF_DEBUG, "FixAdvancePC by: {} to: {:p}", delta, m_address + delta);
        m_address += delta;
        break;
    }
    default:
        dbgln("Unhandled LineProgram opcode {}", opcode);
        VERIFY_NOT_REACHED();
    }
}
void LineProgram::handle_special_opcode(u8 opcode)
{
    u8 adjusted_opcode = opcode - SPECIAL_OPCODES_BASE;
    ssize_t address_increment = (adjusted_opcode / m_unit_header.line_range) * m_unit_header.min_instruction_length;
    ssize_t line_increment = m_unit_header.line_base + (adjusted_opcode % m_unit_header.line_range);

    m_address += address_increment;
    m_line += line_increment;

    if constexpr (DWARF_DEBUG) {
        dbgln("Special adjusted_opcode: {}, address_increment: {}, line_increment: {}", adjusted_opcode, address_increment, line_increment);
        dbgln("Address is now: {:p}, and line is: {}:{}", m_address, m_source_files[m_file_index].name, m_line);
    }

    append_to_line_info();
}

void LineProgram::run_program()
{
    reset_registers();

    while ((size_t)m_stream.offset() < m_unit_offset + sizeof(u32) + m_unit_header.length) {
        u8 opcode = 0;
        m_stream >> opcode;

        dbgln_if(DWARF_DEBUG, "{:p}: opcode: {}", m_stream.offset() - 1, opcode);

        if (opcode == 0) {
            handle_extended_opcode();
        } else if (opcode >= 1 && opcode <= 12) {
            handle_standard_opcode(opcode);
        } else {
            handle_special_opcode(opcode);
        }
    }
}

}
