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
#include "RendererView.h"
#include "MapView.h"
#include <LibGUI/Application.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Window.h>

int main(int argc, char** argv) {
    auto app = GUI::Application::construct(argc, argv);
    auto window = GUI::Window::construct();

    window->set_double_buffering_enabled(false);
    window->set_title("SHOOTER! :^)");
    window->resize(600, 500);

    auto& main_widget = window->set_main_widget<GUI::Widget>();
    main_widget.set_layout<GUI::HorizontalBoxLayout>();

    Game game;

    auto& renderer = main_widget.add<RendererView>(&game);
    auto& map_widget = main_widget.add<MapView>(&game);
    map_widget.set_fixed_width(128);

    renderer.start_timer(16, Core::TimerShouldFireWhenNotVisible::Yes);
    map_widget.start_timer(64, Core::TimerShouldFireWhenNotVisible::Yes);
    renderer.set_focus(true);

    window->show();

    return app->exec();
}