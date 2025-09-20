#include "calendar_panel.hpp"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow main_window;

    auto *calendar_panel = new CalendarPanel(&main_window);
    main_window.setCentralWidget(calendar_panel);
    main_window.show();

    return app.exec();
}
