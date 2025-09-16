#pragma once

#include "const.hpp"
#include "event_creator.hpp"
#include <QGraphicsScene>
#include <QPainter>
#include <QString>
#include <set>

class Calendar : public QGraphicsScene {
    Q_OBJECT
public:
    explicit Calendar(uint8_t hour_start = 8, uint8_t hour_end = 18, QObject *parent = nullptr);
    enum class Location { kNone, kCells, kColumnHeader, kRowHeader };
    void add_event(Event event_data);

private:
    // Size in px
    uint8_t hour_start_;
    uint8_t hour_end_;
    uint8_t hour_blocks_;
    Location identify_location(QPointF point);
    std::multiset<SemiMutablePair<Event, uint8_t>> events_in_week_[kWeekDaysSize];
    uint8_t weekday_split_[kWeekDaysSize] = {0};
    double get_hour_height() { return 60; };
    double get_day_width() { return 160; };
    double get_column_header_height() { return 20; };
    double get_row_header_width() { return 30; };
    bool add_event_data(Event event_data);
    void add_event_graphics(Event event_data);

protected:
    void drawBackground(QPainter *painter, const QRectF &rectangle) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void setSceneRect();
};
