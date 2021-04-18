/*
 * Copyright (c) 2021, the SerenityOS developers.
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

#include <LibGUI/Event.h>
class Game final {
public:
    Game();
    Game(const Game&) = default;
    void on_key(GUI::KeyEvent& event, bool keydown);
    void update();

    float  get_player_x() const { return m_player_x; };
    float  get_player_y() const { return m_player_y; };

    float  get_player_dir_x() const { return m_player_dir_x; };
    float  get_player_dir_y() const { return m_player_dir_y; };

    float  get_player_plane_x() const { return m_player_plane_x; };
    float  get_player_plane_y() const { return m_player_plane_y; };

    u32 get_map_width() const { return m_map_width; }
    u32 get_map_height() const { return m_map_height; }
    const u8* get_map_cells() const { return m_map_cells; }
    u8 get_cell(int x, int y) const { return m_map_cells[x + y * m_map_width ]; }

private:
    bool moving_forward { false };
    bool moving_backward { false };
    bool rotating_right { false };
    bool rotating_left { false };

    float  m_player_x;
    float  m_player_y;
    float  m_player_dir_x;
    float  m_player_dir_y;

    float  m_player_plane_x;
    float m_player_plane_y;

    u32 m_map_width;
    u32 m_map_height;
    u8* m_map_cells;
};
