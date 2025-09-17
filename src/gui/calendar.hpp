#pragma once

#include "const.hpp"
#include "event_creator.hpp"
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QPainter>
#include <QString>
#include <set>

class Event;

class Calendar : public QGraphicsScene {
    Q_OBJECT
public:
    explicit Calendar(uint8_t hour_start = 8, uint8_t hour_end = 18, QObject *parent = nullptr);
    enum class Location { kNone, kCells, kColumnHeader, kRowHeader };
    void add_event(EventData event_data);

private:
    // Size in px
    uint8_t hour_start_;
    uint8_t hour_end_;
    std::multiset<Event *, DereferencedLess<Event>> events_[kWeekDaysSize];
    uint8_t weekday_split_[kWeekDaysSize] = {0};
    QGraphicsTextItem *column_header_[kWeekDaysSize];
    std::vector<QGraphicsTextItem *> row_header_;

    double get_hour_height() const { return 60; };
    double get_day_width() const { return 160; };
    double get_column_header_height() const { return 30; };
    double get_row_header_width() const { return 40; };
    double get_event_x_padding() const { return 1; };
    double get_event_y_padding() const { return 0; };
    QFont get_day_header_font() const { return QFont("Arial", 12); };
    QFont get_hour_header_font() const { return QFont("Arial", 8); };

    double get_time_y_dimension(QTime time);
    double get_day_x_dimension(double day);

    Location identify_location(QPointF point);
    std::vector<std::vector<Event *>> select_event_groups(uint8_t week_day);
    void add_event_graphics(Event *event, uint8_t event_group);

protected:
    void drawBackground(QPainter *painter, const QRectF &rectangle) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void setSceneRect();
};

class Event : public QGraphicsObject {
    Q_OBJECT
    friend Calendar;

public:
    explicit Event(EventData &event, QGraphicsItem *parent = nullptr) : event_data_(event), QGraphicsObject(parent) {};
    std::strong_ordering operator<=>(const Event &other) const { return event_data_ <=> other.event_data_; };
    QRectF boundingRect() const override { return rectangle_; };
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    EventData get_event_data() const { return event_data_; };
signals:
    void clicked(QPointF position);

private:
    void set_rectangle(QRectF rectangle);
    EventData event_data_;
    QRectF rectangle_ = QRectF();
};
