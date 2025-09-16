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
    events_in_week_[event_data.week_day].push(event_data);
    auto event_groups = this->select_event_groups(event_data.week_day);
    for (uint8_t i = 0; i < event_groups.size(); ++i) {
        for (Event event : event_groups[i]) {
            add_event_graphics(event_data, i, event_groups.size());
        }
    }
}

std::vector<std::vector<Event>> Calendar::select_event_groups(uint8_t week_day) {
    auto events_in_day = events_in_week_[week_day];
    std::vector<std::vector<Event>> groups;
    // Assign all events
    while (!events_in_day.empty()) {
        // Define optimization values.
        int minimum_second_difference = 60 * 60 * 24;
        uint8_t best_group = groups.size();
        // Take last event.
        Event event = events_in_day.top();
        events_in_day.pop();
        // Loop through groups
        for (uint8_t i = 0; i < groups.size(); ++i) {
            int time_difference = groups[i].back().end.secsTo(event.start);
            // Look for the group with the minimum difference
            if (time_difference == 0) {
                best_group = i;
                i = groups.size();
            } else if (time_difference > 0 && time_difference < minimum_second_difference) {
                minimum_second_difference = time_difference;
                best_group = i;
            }
        }
        // Create new group if new was not found
        if (best_group == groups.size()) {
            groups.push_back({event});
        } else {
            groups[best_group].push_back(event);
        }
    }
    return groups;
}
