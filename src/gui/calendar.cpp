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
    // Checks input
    assert(hour_end >= hour_start && hour_end <= 24 && hour_start <= 24);
    // Define constants used in function
    const QFont kColumnHeaderFont = QFont("Arial", 12);
    const QFont kRowHeaderFont = QFont("Arial", 8);
    // Define variables
    double calendar_height = this->get_time_y_dimension(QTime(hour_end, 0));
    double calendar_width = this->get_day_x_dimension(kWeekDays.size());
    QGraphicsScene::setSceneRect(0, 0, calendar_width, calendar_height);
    // Set column header text and position it at the centre
    for (uint8_t day = 0; day < kWeekDaysSize; ++day) {
        QGraphicsTextItem *text_item = this->addText(kWeekDays[day], kColumnHeaderFont);
        QRectF text_block = text_item->boundingRect();
        text_item->setPos(get_day_x_dimension(day + 0.5) - text_block.width() / 2,
                          (get_column_header_height() - text_block.height()) / 2);
        column_header_[day] = text_item;
    }
    // Set row header to the upper right corner
    for (uint8_t hour = hour_start_; hour < hour_end_; ++hour) {
        QTime hour_time = QTime(hour, 0);
        QGraphicsTextItem *text_item = this->addText(hour_time.toString("H:mm"), kRowHeaderFont);
        QRectF text_block = text_item->boundingRect();
        text_item->setPos(get_row_header_width() - text_block.width(), get_time_y_dimension(hour_time));
        row_header_.push_back(text_item);
    }
}

void Calendar::drawBackground(QPainter *painter, const QRectF &rectangle) {
    painter->fillRect(rectangle, Qt::white);
    // Draw horizontal lines with quarters.
    for (uint8_t quarter = 4 * hour_start_; quarter <= 4 * hour_end_; ++quarter) {
        // Convert quarter to hour
        double y = this->get_time_y_dimension(QTime(quarter / 4, 15 * (quarter % 4)));
        if (quarter % 4 == 0) {
            painter->setPen(QPen(Qt::blue, 1.5));
            painter->drawLine(rectangle.left(), y, rectangle.right(), y);
        } else {
            painter->setPen(QPen(Qt::blue, 0.5));
            painter->drawLine(this->get_row_header_width(), y, rectangle.right(), y);
        }
    }
    // Draw vertical lines.
    painter->setPen(QPen(Qt::blue, 1.5));
    for (uint8_t day = 0; day <= kWeekDaysSize; ++day) {
        double x = this->get_day_x_dimension(day);
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

Event::Event(EventData &event, QGraphicsItem *parent) : event_data_(event), QGraphicsObject(parent) {
    // Initialize text
    title_text_ = new QGraphicsTextItem(this);
    time_text_ = new QGraphicsTextItem(this);
    // Limit objects the the event block size
    this->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    this->title_text_->setDefaultTextColor(Qt::black);
    this->time_text_->setDefaultTextColor(Qt::black);
}

void Event::set_rectangle(QRectF new_rectangle, QPointF position) {
    prepareGeometryChange();
    this->setPos(position);
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
    // Define constants used in this function
    constexpr double kEventPaddingX = 3;
    constexpr double kEventPaddingY = 3;
    constexpr double kTextPaddingX = 2;
    constexpr double kTextPaddingY = 1;
    EventData data = event->get_event_data();
    // Define dimensions of event
    double x = kEventPaddingX +
               get_day_x_dimension(data.week_day + static_cast<double>(group) / weekday_split_[data.week_day]);
    double y = kEventPaddingY + get_time_y_dimension(data.start);
    double width = get_day_width() / weekday_split_[data.week_day] - 2 * kEventPaddingX;
    double height =
        get_hour_height() * (static_cast<double>(data.start.secsTo(data.end)) / (60 * 60)) - 2 * kEventPaddingY;
    // Sets the box for event
    QRectF rectangle = QRectF(0, 0, width, height);
    QPointF position = QPointF(x, y);
    event->set_rectangle(rectangle, position);
    // Writes text in event
    event->title_text_->setFont(QFont("Inter", static_cast<int>(width * 0.08)));
    event->title_text_->setPlainText(event->event_data_.title);
    event->title_text_->setPos(kTextPaddingX, kTextPaddingY);
    event->title_text_->setTextWidth(width - 2 * kTextPaddingX);

    event->time_text_->setFont(QFont("Inter", static_cast<int>(width * 0.06)));
    event->time_text_->setPlainText(event->event_data_.start.toString("H:mm") + " - " +
                                    event->event_data_.end.toString("H:mm"));
    event->time_text_->setPos(kTextPaddingX, event->title_text_->boundingRect().height());
    event->time_text_->setTextWidth(width - 2 * kTextPaddingX);
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
