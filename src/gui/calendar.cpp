#include "calendar.hpp"
#include "const.hpp"
#include "event_creator.hpp"
#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <algorithm>
#include <assert.h>

Calendar::Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent)
    : QGraphicsScene(parent), hour_start_(hour_start), hour_end_(hour_end), hour_blocks_(hour_end - hour_start - 1) {
    assert(hour_end >= hour_start && hour_end <= 24 && hour_start <= 24);
    double calendar_height = this->hour_blocks_ * this->get_hour_height() + this->get_column_header_height();
    double calendar_width = this->get_day_width() * kWeekDays.size() + this->get_row_header_width();
    QGraphicsScene::setSceneRect(0, 0, calendar_width, calendar_height);
}

void Calendar::drawBackground(QPainter *painter, const QRectF &rectangle) {
    painter->fillRect(rectangle, Qt::white);
    painter->setPen(QPen(Qt::blue, 1.5));
    // Draw horizontal lines.
    for (uint8_t hour_block = 0; hour_block <= this->hour_blocks_; ++hour_block) {
        double y = hour_block * this->get_hour_height() + this->get_column_header_height();
        painter->drawLine(rectangle.left(), y, rectangle.right(), y);
    }
    // Draw vertical lines.
    for (uint8_t day = 0; day <= kWeekDays.size(); ++day) {
        double x = day * this->get_day_width() + this->get_row_header_width();
        painter->drawLine(x, rectangle.top(), x, rectangle.bottom());
    }
}

Calendar::Location Calendar::identify_location(QPointF point) {
    // Outide calendar or empty square in top left.
    if (!sceneRect().contains(point) ||
        (point.x() < this->get_row_header_width() && point.y() < this->get_column_header_height())) {
        return Calendar::Location::kNone;
    } else if (point.x() >= this->get_row_header_width() && point.y() < this->get_column_header_height()) {
        return Calendar::Location::kColumnHeader;
    } else if (point.x() < this->get_row_header_width() && point.y() >= this->get_column_header_height()) {
        return Calendar::Location::kRowHeader;
    } else {
        return Calendar::Location::kCells;
    }
}

void Calendar::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    const QPointF position = event->scenePos();
    if (this->identify_location(position) != Calendar::Location::kCells) {
        return;
    }
    // identify day
    uint8_t day = (position.x() - this->get_row_header_width()) / this->get_day_width();
    // identify time
    uint8_t hour_shift = (position.y() - this->get_column_header_height()) / this->get_hour_height();
    uint8_t quarter = 4 * ((position.y() - this->get_column_header_height()) / this->get_hour_height() - hour_shift);
    QTime time(hour_shift + hour_start_, 15 * quarter);
    // Show event creator
    EventCreator event_creator(day, time, QApplication::activeWindow());
    if (event_creator.exec() == QDialog::Accepted) {
        this->add_event(event_creator.get_data());
    }
}

void Calendar::add_event(Event event_data) {
    bool need_repaint = this->add_event_data(event_data);
    // Draw only new event if it can
    if (!need_repaint) {
        this->add_event_graphics(event_data);
    } else {
        std::multiset<SemiMutablePair<Event, uint8_t>> &events_in_day = events_in_week_[event_data.week_day];
        for (auto it = events_in_day.begin(); it != events_in_day.end(); ++it) {
            this->add_event_graphics(event_data);
        }
    }
}

// Fix
//
// Too much shifting in position. Can fit new event without shifting next collisions.
//
// Fix
bool Calendar::add_event_data(Event event_data) {
    std::multiset<SemiMutablePair<Event, uint8_t>> &events_in_day = events_in_week_[event_data.week_day];
    uint8_t new_split = weekday_split_[event_data.week_day];
    auto event_iterator = events_in_day.insert({event_data, 1});
    bool changed = false;
    // Calculate new collisions with earlier events
    auto backward_iterator = event_iterator;
    --backward_iterator;
    while (backward_iterator != events_in_day.begin()) {
        if (event_iterator->first.start < backward_iterator->first.end) {
            // Increase collisions
            ++event_iterator->second;
            ++backward_iterator->second;
            // Update positions shift
            ++event_iterator->first.position;
            new_split = std::max(backward_iterator->second, new_split);
            --backward_iterator;
        }
    }
    // Calculate new colitions with later events
    auto forward_iterator = event_iterator;
    ++forward_iterator;
    while (forward_iterator != events_in_day.end()) {
        if (event_iterator->first.end > backward_iterator->first.start) {
            // Increase collisions
            ++event_iterator->second;
            ++forward_iterator->second;
            // Update positions shift
            ++forward_iterator->first.position;
            new_split = std::max(forward_iterator->second, new_split);
        } else {
            forward_iterator = events_in_day.end();
        }
        ++forward_iterator;
    }
    // Update split
    new_split = std::max(event_iterator->second, new_split);
    if (new_split != weekday_split_[event_data.week_day]) {
        changed = true;
    }
    return changed;
}
