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

#include "RendererView.h"
#include <LibGUI/Painter.h>


RendererView::RendererView(Game* game)
    : m_game(game)
    , m_buggie_sprite(*Gfx::Bitmap::load_from_file("/res/graphics/shooter/buggie.png"))
{
    resize(rect().width(), rect().height());
}

RendererView::~RendererView()
{
}

struct RayResult {
    float perpendicular_distance;
    int side;
};

RayResult RendererView::ray_cast(int x)
{
    int width = m_framebuffer_size.width();
    float camera_x = 2 * x / (float)width - 1;
    float ray_dir_x = m_game->get_player_dir_x() + m_game->get_player_plane_x() * camera_x;
    float ray_dir_y = m_game->get_player_dir_y() + m_game->get_player_plane_y() * camera_x;

    float pos_x = m_game->get_player_x();
    float pos_y = m_game->get_player_y();

    int map_x = (int)pos_x;
    int map_y = (int)pos_y;

    float side_dist_x;
    float side_dist_y;

    float delta_dist_x = fabsf(1 / ray_dir_x);
    float delta_dist_y = fabsf(1 / ray_dir_y);

    float perp_wall_dist;

    int step_x;
    int step_y;

    int hit = 0;
    int side;

    if (ray_dir_x < 0) {
        step_x = -1;
        side_dist_x = (pos_x - (float)map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = ((float)map_x + 1.0f - pos_x) * delta_dist_x;
    }
    if (ray_dir_y < 0) {
        step_y = -1;
        side_dist_y = (pos_y - (float)map_y) * delta_dist_y;
    } else {
        step_y = 1;
        side_dist_y = ((float)map_y + 1.0f - pos_y) * delta_dist_y;
    }

    while (hit == 0) {
        if (side_dist_x < side_dist_y) {
            side_dist_x += delta_dist_x;
            map_x += step_x;
            side = 0;
        } else {
            side_dist_y += delta_dist_y;
            map_y += step_y;
            side = 1;
        }

        if (m_game->get_cell(map_x, map_y) > 0)
            hit = 1;
    }

    if (side == 0)
        perp_wall_dist = fabsf(((float)map_x - pos_x + (1.0f - (float)step_x) / 2.0f) / ray_dir_x);
    else
        perp_wall_dist = fabsf(((float)map_y - pos_y + (1.0f - (float)step_y) / 2.0f) / ray_dir_y);

    return { .perpendicular_distance = perp_wall_dist, .side = side };
}

void RendererView::paint_event(GUI::PaintEvent&)
{
    GUI::Painter painter(*this);

    for (int y = 0; y < rect().height(); y++) {
        for (int x = 0; x < rect().width(); x++) {
            painter.set_pixel(x, y, get_pixel(x * m_framebuffer_size.width() / rect().width(), y * m_framebuffer_size.height() / rect().height()));
        }
    }
}

void RendererView::timer_event(Core::TimerEvent&)
{
    m_game->update();

    VERIFY(m_game != nullptr);

    // Paint background
    auto my_rect = m_framebuffer_size;
    for (int x = 0; x < my_rect.width(); x++) {
        for (int y = 0; y < my_rect.height(); y++) {
            if (y > my_rect.height() / 2) {
                set_pixel(x, y, Color::NamedColor::DarkGray);
            } else {
                set_pixel(x, y, Color::NamedColor::LightGray);
            }
            set_depth(x, y, 1000);
        }
    }

    // Paint 3D World
    for (int x = 0; x < my_rect.width(); x++) {
        // cast rays for each column of the screen
        auto ray = ray_cast(x);

        if (ray.perpendicular_distance < 0) {
            dbgln("Perp Dist is {}", ray.perpendicular_distance);
        }

        int line_height = (int)(my_rect.height() / ray.perpendicular_distance);

        int draw_start = -line_height / 2 + my_rect.height() / 2;
        if (draw_start < 0)
            draw_start = 0;
        int draw_end = line_height / 2 + my_rect.height() / 2;
        if (draw_end >= my_rect.height())
            draw_end = my_rect.height();

        auto color = Color(255, 0, 0);
        if (ray.side == 1)
            color = Color(128, 0, 0);

        for (int y = draw_start; y < draw_end; y++) {
            set_pixel(x, y, color);
            set_depth(x, y, ray.perpendicular_distance);
        }
    }

    float m_buggie_pos_x = 16 - m_game->get_player_x();
    float m_buggie_pos_y = 16 - m_game->get_player_y();

    auto planeX = m_game->get_player_plane_x();
    auto planeY = m_game->get_player_plane_y();
    auto dirX = m_game->get_player_dir_x();
    auto dirY = m_game->get_player_dir_y();

    float invDet = 1.0f / (planeX * dirY - dirX * planeY); //required for correct matrix multiplication

    float transformX = invDet * (dirY * m_buggie_pos_x - dirX * m_buggie_pos_y);
    float transformY = invDet * (-planeY * m_buggie_pos_x + planeX * m_buggie_pos_y); //this is actually the depth inside the screen, that what Z is in 3D

    int spriteScreen_x = int((my_rect.width() / 2) * (1 + transformX / transformY));

    //calculate height of the sprite on screen
    int spriteHeight = abs(int(my_rect.height() / (transformY))); //using 'transformY' instead of the real distance prevents fisheye
    //calculate lowest and highest pixel to fill in current stripe
    int drawStartY = -spriteHeight / 2 + my_rect.height() / 2;
    if (drawStartY < 0)
        drawStartY = 0;
    int drawEndY = spriteHeight / 2 + my_rect.height() / 2;
    if (drawEndY >= my_rect.height())
        drawEndY = my_rect.height() - 1;

    //calculate width of the sprite
    int spriteWidth = abs(int(my_rect.height() / (transformY)));
    int drawStartX = -spriteWidth / 2 + spriteScreen_x;
    if (drawStartX < 0)
        drawStartX = 0;
    int drawEndX = spriteWidth / 2 + spriteScreen_x;
    if (drawEndX >= my_rect.width())
        drawEndX = my_rect.width() - 1;

    int texWidth = m_buggie_sprite->width();
    int texHeight = m_buggie_sprite->height();

    for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
        int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreen_x)) * texWidth / spriteWidth) / 256;
        if (transformY > 0 && stripe > 0 && stripe < my_rect.width()) {
            for (int y = drawStartY; y < drawEndY; y++) {
                int d = (y)*256 - my_rect.height() * 128 + spriteHeight * 128; //256 and 128 factors to avoid floats
                int texY = ((d * texHeight) / spriteHeight) / 256;

                if(texX < 0 || texY < 0 || texX >= m_buggie_sprite->width() || texY >= m_buggie_sprite->height())
                    continue;

                auto pixel = m_buggie_sprite->get_pixel(texX, texY);
                auto depth_buffer = get_depth(stripe, y);
                if (pixel.alpha() > 0 && transformY < depth_buffer)
                    set_pixel(stripe, y, pixel);
            }
        }
    }

    update();
}

void RendererView::keydown_event(GUI::KeyEvent& event)
{
    m_game->on_key(event, true);
}

void RendererView::keyup_event(GUI::KeyEvent& event)
{
    m_game->on_key(event, false);
}

void RendererView::resize_event(GUI::ResizeEvent& event)
{
    resize(event.size().width(), event.size().height());
}

void RendererView::resize(int width, int height) {
    delete m_framebuffer;
    delete m_depth_buffer;

    m_framebuffer_size = { (int)((float)width * RESOLUTION_SCALE), (int)((float)height * RESOLUTION_SCALE) };

    m_framebuffer = new Gfx::Color[m_framebuffer_size.width() * m_framebuffer_size.height()];
    m_depth_buffer = new float[m_framebuffer_size.width() * m_framebuffer_size.height()];
}