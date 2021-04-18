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

#include "Game.h"
#include <LibGUI/Frame.h>

struct RayResult;

class RendererView final : public GUI::Frame {
    C_OBJECT(RendererView);

public:
    virtual ~RendererView() override;

private:
    explicit RendererView(Game*);
    virtual void keydown_event(GUI::KeyEvent&) override;
    virtual void keyup_event(GUI::KeyEvent&) override;
    virtual void paint_event(GUI::PaintEvent&) override;
    virtual void timer_event(Core::TimerEvent&) override;
    virtual void resize_event(GUI::ResizeEvent&) override;

    void set_pixel(int x, int y, Gfx::Color color) { m_framebuffer[x + y * m_framebuffer_size.width()] = color; }
    Gfx::Color get_pixel(int x, int y) { return m_framebuffer[x + y * m_framebuffer_size.width()]; }
    void set_depth(int x, int y, float depth) { m_depth_buffer[x + y * m_framebuffer_size.width()] = depth; }
    float get_depth(int x, int y) { return m_depth_buffer[x + y * m_framebuffer_size.width()]; }

    RayResult ray_cast(int x);
    void resize(int width, int height);

    Game* m_game { nullptr };
    NonnullRefPtr<Gfx::Bitmap> m_buggie_sprite;

    Gfx::IntSize m_framebuffer_size;
    Gfx::Color* m_framebuffer{ nullptr };
    float* m_depth_buffer{ nullptr };
    const float RESOLUTION_SCALE = 0.25f;
};