/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 * Copyright (c) 2021, Idan Horowitz <idan.horowitz@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Math.h>
#include <LibCore/System.h>
#include <LibGUI/Action.h>
#include <LibGUI/Application.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/ConnectionToWindowServer.h>
#include <LibGUI/Frame.h>
#include <LibGUI/Icon.h>
#include <LibGUI/Menu.h>
#include <LibGUI/Menubar.h>
#include <LibGUI/Painter.h>
#include <LibGUI/Widget.h>
#include <LibGUI/Window.h>
#include <LibGfx/Bitmap.h>
#include <LibGfx/Path.h>
#include <LibMain/Main.h>
#include <unistd.h>

class MainFrame final : public GUI::Frame {
    C_OBJECT(MainFrame);

public:
    virtual void timer_event(Core::TimerEvent&) override
    {

        m_show_scroll_wheel = false;
        m_double_clicked_buttons = 0;
        stop_timer();
        update();
    }

    virtual void paint_event(GUI::PaintEvent& event) override
    {
        GUI::Painter painter(*this);
        painter.add_clip_rect(event.rect());
        painter.fill_rect(frame_inner_rect(), Color::White);

        auto buttons = m_buttons | m_double_clicked_buttons;

        auto primary_secondary_switched = GUI::ConnectionToWindowServer::the().are_mouse_buttons_switched();
        auto primary_pressed = buttons & GUI::MouseButton::Primary;
        auto secondary_pressed = buttons & GUI::MouseButton::Secondary;

        Gfx::Path path;

        // draw left button
        path.move_to({ 30, 65 });
        path.line_to({ 30, 20 });
        path.line_to({ 65, 12 });
        path.line_to({ 65, 65 });
        if (primary_secondary_switched ? secondary_pressed : primary_pressed) {
            auto button_color = primary_secondary_switched ? Color::LightBlue : Color::MidBlue;
            painter.fill_path(path, button_color);
        }
        painter.stroke_path(path, Color::Black, 1);
        path.clear();

        // draw right button
        path.move_to({ 95, 65 });
        path.line_to({ 95, 12 });
        path.line_to({ 130, 20 });
        path.line_to({ 130, 65 });
        if (primary_secondary_switched ? primary_pressed : secondary_pressed) {
            auto button_color = primary_secondary_switched ? Color::MidBlue : Color::LightBlue;
            painter.fill_path(path, button_color);
        }
        painter.stroke_path(path, Color::Black, 1);
        path.clear();

        // draw middle button
        path.move_to({ 65, 65 });
        path.line_to({ 65, 12 });
        path.line_to({ 95, 12 });
        path.line_to({ 95, 65 });
        if (buttons & GUI::MouseButton::Middle)
            painter.fill_path(path, Color::Blue);
        painter.stroke_path(path, Color::Black, 1);
        path.clear();

        if (m_show_scroll_wheel) {
            auto radius = 10;
            auto off_x = 80;
            auto off_y = 38;

            Gfx::IntPoint p1;
            Gfx::IntPoint p2;
            Gfx::IntPoint p3;
            Gfx::IntPoint p4;

            p1.set_x(radius * AK::cos(AK::Pi<double> * m_wheel_delta_acc / 18.) + off_x);
            p1.set_y(radius * AK::sin(AK::Pi<double> * m_wheel_delta_acc / 18.) + off_y);

            p2.set_x(radius * AK::cos(AK::Pi<double> * (m_wheel_delta_acc + 18) / 18.) + off_x);
            p2.set_y(radius * AK::sin(AK::Pi<double> * (m_wheel_delta_acc + 18) / 18.) + off_y);

            p3.set_x(radius * AK::cos(AK::Pi<double> * (m_wheel_delta_acc + 9) / 18.) + off_x);
            p3.set_y(radius * AK::sin(AK::Pi<double> * (m_wheel_delta_acc + 9) / 18.) + off_y);

            p4.set_x(radius * AK::cos(AK::Pi<double> * (m_wheel_delta_acc + 27) / 18.) + off_x);
            p4.set_y(radius * AK::sin(AK::Pi<double> * (m_wheel_delta_acc + 27) / 18.) + off_y);

            painter.draw_line(p1, p2, Color::Red, 2);
            painter.draw_line(p3, p4, Color::Red, 2);
        }

        // draw mouse outline
        path.move_to({ 30, 140 });
        path.line_to({ 30, 65 });
        path.line_to({ 130, 65 });
        path.line_to({ 130, 140 });
        path.line_to({ 30, 140 });
        painter.stroke_path(path, Color::Black, 1);
        path.clear();

        // draw forward button
        path.move_to({ 30, 43 });
        path.line_to({ 25, 43 });
        path.line_to({ 25, 60 });
        path.line_to({ 30, 60 });
        if (buttons & GUI::MouseButton::Forward)
            painter.fill_rect({ 26, 44, 4, 16 }, Color::Blue);
        painter.stroke_path(path, Color::Black, 1);
        path.clear();

        // draw back button
        path.move_to({ 30, 70 });
        path.line_to({ 25, 70 });
        path.line_to({ 25, 87 });
        path.line_to({ 30, 87 });
        if (buttons & GUI::MouseButton::Backward)
            painter.fill_rect({ 26, 71, 4, 16 }, Color::Blue);
        painter.stroke_path(path, Color::Black, 1);
        path.clear();
    }

    void mousedown_event(GUI::MouseEvent& event) override
    {
        m_buttons = event.buttons();
        update();
    }

    void mouseup_event(GUI::MouseEvent& event) override
    {
        m_buttons = event.buttons();
        update();
    }

    void mousewheel_event(GUI::MouseEvent& event) override
    {
        m_wheel_delta_acc = (m_wheel_delta_acc + event.wheel_delta_y() + 36) % 36;
        m_show_scroll_wheel = true;
        update();
        if (has_timer())
            stop_timer();
        start_timer(500);
    }

    void doubleclick_event(GUI::MouseEvent& event) override
    {
        m_double_clicked_buttons |= event.button();
        update();
        if (has_timer())
            stop_timer();
        start_timer(500);
    }

private:
    unsigned m_buttons { 0 };
    unsigned m_wheel_delta_acc { 0 };
    bool m_show_scroll_wheel { false };
    unsigned m_double_clicked_buttons { 0 };
};

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    auto app = TRY(GUI::Application::try_create(arguments));
    auto app_icon = TRY(GUI::Icon::try_create_default_icon("app-mouse"sv));

    TRY(Core::System::pledge("stdio recvfd sendfd rpath"));
    TRY(Core::System::unveil("/res", "r"));
    TRY(Core::System::unveil(nullptr, nullptr));

    auto window = TRY(GUI::Window::try_create());
    window->set_title("Mouse demo");
    window->set_icon(app_icon.bitmap_for_size(16));
    window->resize(160, 155);

    auto main_widget = TRY(window->try_set_main_widget<MainFrame>());
    main_widget->set_fill_with_background_color(true);

    auto file_menu = TRY(window->try_add_menu("&File"));
    TRY(file_menu->try_add_action(GUI::CommonActions::make_quit_action([&](auto&) { app->quit(); })));

    auto help_menu = TRY(window->try_add_menu("&Help"));
    TRY(help_menu->try_add_action(GUI::CommonActions::make_command_palette_action(window)));
    TRY(help_menu->try_add_action(GUI::CommonActions::make_about_action("Mouse Demo", app_icon, window)));

    window->set_resizable(false);
    window->show();
    return app->exec();
}
