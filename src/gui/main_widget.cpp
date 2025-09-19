#include "calendar.hpp"
#include <QApplication>
#include <QGraphicsView>
#include <QTableView>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    auto *view = new QGraphicsView;
    auto *calendar = new Calendar;
    view->setScene(calendar);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setFixedSize(calendar->sceneRect().size().toSize());
    view->show();
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    return app.exec();
}
