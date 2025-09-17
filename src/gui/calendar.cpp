#include "calendar.hpp"
#include "const.hpp"
#include "event_creator.hpp"
#include <QApplication>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <assert.h>

Calendar::Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent)
    : QGraphicsScene(parent), hour_start_(hour_start), hour_end_(hour_end) {
    assert(hour_end >= hour_start && hour_end <= 24 && hour_start <= 24);
    double calendar_height = this->get_time_y_dimension(QTime(hour_end, 0));
    double calendar_width = this->get_day_x_dimension(kWeekDays.size());
    QGraphicsScene::setSceneRect(0, 0, calendar_width, calendar_height);
}

void Calendar::drawBackground(QPainter *painter, const QRectF &rectangle) {
    painter->fillRect(rectangle, Qt::white);
    painter->setPen(QPen(Qt::blue, 1.5));
    // Draw horizontal lines.
    for (uint8_t hour = hour_start_; hour <= hour_end_; ++hour) {
        double y = get_time_y_dimension(QTime(hour, 0));
        painter->drawLine(rectangle.left(), y, rectangle.right(), y);
    }
    // Draw vertical lines.
    for (uint8_t day = 0; day <= kWeekDaysSize; ++day) {
        double x = get_day_x_dimension(day);
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

double Calendar::get_time_y_dimension(QTime time) {
    double y = this->get_column_header_height() +
               this->get_hour_height() * ((time.hour() - hour_start_) + static_cast<double>(time.minute()) / 60);
    return y;
}

double Calendar::get_day_x_dimension(double day) {
    double x = this->get_row_header_width() + this->get_day_width() * day;
    return x;
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

void Event::set_rectangle(QRectF new_rectangle) {
    prepareGeometryChange();
    rectangle_ = new_rectangle;
    update();
}

void Event::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // Check if is valid
    QRectF rectangle = boundingRect();
    if (!rectangle.isValid()) {
        return;
    }
    painter->setBrush(Qt::red);
    painter->setPen(QPen(Qt::red, 1));
    painter->drawRect(boundingRect());
}

void Calendar::add_event(EventData event_data) {
    Event *new_event = new Event(event_data);
    events_[event_data.week_day].insert(new_event);
    // Divides into subcolumns and show them
    std::vector<std::vector<Event *>> event_groups = this->select_event_groups(event_data.week_day);
    for (uint8_t i = 0; i < event_groups.size(); ++i) {
        for (Event *event : event_groups[i]) {
            this->add_event_graphics(event, i);
        }
    }
    this->addItem(new_event);
}

void Calendar::add_event_graphics(Event *event, uint8_t group) {
    EventData data = event->get_event_data();
    // Define dimensions of event
    double x = get_event_x_padding() +
               get_day_x_dimension(data.week_day + static_cast<double>(group) / weekday_split_[data.week_day]);
    double y = get_event_y_padding() + get_time_y_dimension(data.start);
    double width = get_day_width() / weekday_split_[data.week_day] - get_event_x_padding();
    double height = get_hour_height() * (static_cast<double>(data.start.secsTo(data.end)) / (60 * 60));
    QRectF rectangle = QRectF(x, y, width, height);
    event->set_rectangle(rectangle);
}

std::vector<std::vector<Event *>> Calendar::select_event_groups(uint8_t week_day) {
    std::vector<std::vector<Event *>> groups;
    // Assign all events
    for (auto *event : events_[week_day]) {
        // Get data
        EventData event_data = event->get_event_data();
        // Define optimization values.
        int minimum_second_difference = 60 * 60 * 24;
        uint8_t best_group = groups.size();
        // Loop through groups
        for (uint8_t i = 0; i < groups.size(); ++i) {
            int time_difference = groups[i].back()->get_event_data().end.secsTo(event_data.start);
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
    weekday_split_[week_day] = groups.size();
    return groups;
}
