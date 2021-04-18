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

#include "Game.h"

Game::Game()
{
    m_map_width = 24;
    m_map_height = 24;

    m_map_cells = new u8[24 * 24] {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 2, 0, 2, 2, 0, 0, 0, 0, 3, 0, 3, 0, 3, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 4, 0, 0, 0, 0, 5, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 4, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 4, 0, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    m_player_x = 22;
    m_player_y = 11.5;

    m_player_dir_x = -1;
    m_player_dir_y = 0;

    m_player_plane_x = 0;
    m_player_plane_y = 0.66;
}

void Game::on_key(GUI::KeyEvent& event, bool key_down)
{
    if (event.key() == Key_W) {
        moving_forward = key_down;
    } else if (event.key() == Key_S) {
        moving_backward = key_down;
    }

    if (event.key() == Key_A) {
        rotating_left = key_down;
    } else if (event.key() == Key_D) {
        rotating_right = key_down;
    }

    //dbgln("Game state after event \nMoving Forward {}\nMoving Backward {}\nRotating Left {}\nRotating Right {}", moving_forward, moving_backward, rotating_left, rotating_right);
}

void Game::update()
{
    float move_speed = 0.05;
    float rot_speed = 0.1;

    float new_pos_x = 0;
    float new_pos_y = 0;

    if (moving_forward) {
        new_pos_x = m_player_x + m_player_dir_x * move_speed;
        new_pos_y = m_player_y + m_player_dir_y * move_speed;
    } else if (moving_backward) {
        new_pos_x = m_player_x - m_player_dir_x * move_speed;
        new_pos_y = m_player_y - m_player_dir_y * move_speed;
    }

    if (rotating_right) {
        float oldDirX = m_player_dir_x;
        m_player_dir_x = m_player_dir_x * cosf(-rot_speed) - m_player_dir_y * sinf(-rot_speed);
        m_player_dir_y = oldDirX * sinf(-rot_speed) + m_player_dir_y * cosf(-rot_speed);

        float oldPlaneX = m_player_plane_x;
        m_player_plane_x = m_player_plane_x * cosf(-rot_speed) - m_player_plane_y * sinf(-rot_speed);
        m_player_plane_y = oldPlaneX * sinf(-rot_speed) + m_player_plane_y * cosf(-rot_speed);
    } else if (rotating_left) {
        float oldDirX = m_player_dir_x;
        m_player_dir_x = m_player_dir_x * cosf(rot_speed) - m_player_dir_y * sinf(rot_speed);
        m_player_dir_y = oldDirX * sinf(rot_speed) + m_player_dir_y * cosf(rot_speed);

        float oldPlaneX = m_player_plane_x;
        m_player_plane_x = m_player_plane_x * cosf(rot_speed) - m_player_plane_y * sinf(rot_speed);
        m_player_plane_y = oldPlaneX * sinf(rot_speed) + m_player_plane_y * cosf(rot_speed);
    }


    if(new_pos_x <= 0)
        new_pos_x = 0;
    if(new_pos_x > m_map_width)
        new_pos_x = m_map_width - 1;

    if(new_pos_y <= 0)
        new_pos_y = 0;
    if(new_pos_y > m_map_height)
        new_pos_y = m_map_height - 1;

    if (m_map_cells[(int)new_pos_x + m_map_width * (int)m_player_y] == false)
        m_player_x = new_pos_x;
    if (m_map_cells[(int)m_player_x + m_map_width * (int)new_pos_y] == false)
        m_player_y = new_pos_y;
}
