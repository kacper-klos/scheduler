#include "calendar.hpp"
#include <QApplication>
#include <QTableView>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Calendar calendar(6, 18);
    QTableView calendar_view;

    calendar_view.setModel(&calendar);
    calendar_view.resize(600, 400);
    calendar_view.show();

    return app.exec();
}
