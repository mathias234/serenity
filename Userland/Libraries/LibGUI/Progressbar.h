/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
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

#include <LibGUI/Frame.h>

namespace GUI {

class Progressbar : public Frame {
    C_OBJECT(Progressbar)
public:
    virtual ~Progressbar() override;

    void set_range(int min, int max);
    void set_min(int min) { set_range(min, max()); }
    void set_max(int max) { set_range(min(), max); }
    void set_value(int);

    int value() const { return m_value; }
    int min() const { return m_min; }
    int max() const { return m_max; }

    void set_orientation(Orientation value);
    Orientation orientation() const { return m_orientation; }

    String text() const { return m_text; }
    void set_text(String text) { m_text = move(text); }

    enum Format {
        NoText,
        Percentage,
        ValueSlashMax
    };
    Format format() const { return m_format; }
    void set_format(Format format) { m_format = format; }

protected:
    Progressbar(Orientation = Orientation::Horizontal);

    virtual void paint_event(PaintEvent&) override;

private:
    Format m_format { Percentage };
    int m_min { 0 };
    int m_max { 100 };
    int m_value { 0 };
    String m_text;
    Orientation m_orientation { Orientation::Horizontal };
};

class VerticalProgressbar final : public Progressbar {
    C_OBJECT(VerticalProgressbar);

public:
    virtual ~VerticalProgressbar() override { }

private:
    VerticalProgressbar()
        : Progressbar(Orientation::Vertical)
    {
    }
};

class HorizontalProgressbar final : public Progressbar {
    C_OBJECT(HorizontalProgressbar);

public:
    virtual ~HorizontalProgressbar() override { }

private:
    HorizontalProgressbar()
        : Progressbar(Orientation::Horizontal)
    {
    }
};

}
