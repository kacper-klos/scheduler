#include "calendar.hpp"
#include "const.hpp"
#include <QPainter>
#include <assert.h>

Calendar::Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent)
    : QGraphicsScene(parent), hour_start_(hour_start), hour_end_(hour_end), hour_blocks_(hour_end - hour_start - 1) {
    assert(hour_end >= hour_start && hour_end <= 24 && hour_start <= 24);
    double calendar_height = this->hour_blocks_ * this->hour_height_ + this->column_header_height;
    double calendar_width = this->day_width_ * kWeekDays.size() + this->row_header_width;
    QGraphicsScene::setSceneRect(0, 0, calendar_width, calendar_height);
}

void Calendar::drawBackground(QPainter *p, const QRectF &r) {
    p->fillRect(r, Qt::white);
    p->setPen(QPen(Qt::blue, 1.5));
    // Draw horizontal lines
    for (uint8_t hour_block = 0; hour_block <= this->hour_blocks_; ++hour_block) {
        double y = hour_block * this->hour_height_ + this->column_header_height;
        p->drawLine(r.left(), y, r.right(), y);
    }
    // Draw vertical lines
    for (uint8_t day = 0; day <= kWeekDays.size(); ++day) {
        double x = day * this->day_width_ + this->row_header_width;
        p->drawLine(x, r.top(), x, r.bottom());
    }
}
