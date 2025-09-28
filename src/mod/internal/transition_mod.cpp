/*
SPDX-FileCopyrightText: 2025 Wallix Proxies Team
SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mod/internal/transition_mod.hpp"
#include "gdi/text.hpp"
#include "gdi/draw_utils.hpp"
#include "core/font.hpp"
#include "utils/theme.hpp"

TransitionMod::TransitionMod(
    chars_view message,
    gdi::GraphicApi & drawable,
    uint16_t width, uint16_t height,
    Rect const widget_rect, ClientExecute & rail_client_execute, Font const& font,
    Theme const& theme
)
    : RailInternalModBase(drawable, width, height, rail_client_execute, font, theme, nullptr)
    , ttmessage(font, message)
    , drawable(drawable)
    , widget_rect(widget_rect)
    , font_height(font.max_height())
    , fgcolor(Widget::Color(theme.tooltip.fgcolor))
    , bgcolor(Widget::Color(theme.tooltip.bgcolor))
    , border_color(Widget::Color(theme.tooltip.border_color))
{
    this->set_mod_signal(BACK_EVENT_NONE);
    this->rdp_input_invalidate(widget_rect);
}

void TransitionMod::rdp_input_invalidate(Rect r)
{
    Rect const clip = r.intersect(widget_rect);

    if (!clip.isempty()) {
        constexpr int padding = 20;
        constexpr int border_len = 1;

        int width = ttmessage.width() + padding * 2;
        int height = font_height + padding * 2;
        int x = widget_rect.x + (widget_rect.cx - width) / 2;
        int y = widget_rect.y + (widget_rect.cy - height) / 2;

        gdi::draw_text(
            drawable,
            x,
            y,
            font_height,
            gdi::DrawTextPadding::Padding{padding - border_len},
            ttmessage.fcs(),
            fgcolor,
            bgcolor,
            clip
        );

        Rect area(checked_int<int>{x}, checked_int<int>{y}, checked_int<int>{width}, checked_int<int>{height});
        gdi_draw_border(
            drawable, border_color, area, border_len, clip, gdi::ColorCtx::depth24()
        );
    }
}

void TransitionMod::rdp_input_scancode(
    KbdFlags flags, Scancode scancode, uint32_t event_time, Keymap const& keymap)
{
    RailModBase::check_alt_f4(keymap);

    if (pressed_scancode(flags, scancode) == Scancode::Esc) {
        this->set_mod_signal(BACK_EVENT_STOP);
    }
    else {
        this->screen.rdp_input_scancode(flags, scancode, event_time, keymap);
    }
}
