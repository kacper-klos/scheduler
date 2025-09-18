// @file calendar.hpp
// @brief User interface for calendar and it events.
//
// Units:
// - Pixels
// Ownership:
// - Every item created by Calendar is owned by it.

#pragma once

#include "const.hpp"
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QPainter>
#include <QString>
#include <QTime>
#include <compare>
#include <set>
#include <tuple>

class Calendar;

// @class Event
// @brief Graphical representation of a single event in Calendar class.
class Event : public QGraphicsObject {
    Q_OBJECT
    friend Calendar;

public:
    // @struct EventData
    // @brief Structure allowing easy transfer of information about the event.
    //
    // Value type: comparable, copayable.
    //
    // Fields:
    // - title: Title of event
    // - week_day: 0=Monday ... 6=Sunday.
    // - start: Time at which event started.
    // - end: Time at which event ended.
    //
    // @note This struct is independent of graphics, used as a storage or comparison.
    struct EventData {
        QString title;
        uint8_t week_day;
        QTime start;
        QTime end;
        // @brief Helper function for comparator.
        //
        // Declares the order of comparison: start, end, title, week_day.
        auto key() const {
            return std::make_tuple(start.hour(), start.minute(), end.hour(), end.minute(), title.toStdString(),
                                   week_day);
        }
        // @brief Declares ordering based on @ref key().
        std::strong_ordering operator<=>(const EventData &other) const { return key() <=> other.key(); }
    };
    // @brief Constructor of a graphical event existing on the calendar
    // @param event_data Information about the event.
    // @param parent The owner of the event.
    explicit Event(EventData &event_data, QGraphicsItem *parent = nullptr);
    // @brief Defines ordering based on @ref EventData ordering.
    std::strong_ordering operator<=>(const Event &other) const { return event_data_ <=> other.event_data_; };
    // @brief Return the visiual rectangle of the event.
    //
    // @sa QGraphicsObject::boundingRect()
    QRectF boundingRect() const override { return rectangle_; };
    // @brief Defines the style of the rectangle and its text.
    //
    // This function is called by the Qt each time the Event needs a repaint.
    //
    // @param painter Use for drawing the style.
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;
    // @brief Getter of event_data_
    EventData get_event_data() const { return event_data_; };

private:
    // @brief Set new rectangle and position of the Event.
    // @param rectangle New visiual base of Event.
    // @param position Pass to owner for a new location.
    void set_rectangle(QRectF rectangle, QPointF position);
    // Information about the event.
    EventData event_data_;
    // Base rectangle with Event visiuals.
    QRectF rectangle_ = QRectF();
    // Texts shown on the Event visiuals.
    QGraphicsTextItem *title_text_ = nullptr;
    QGraphicsTextItem *time_text_ = nullptr;
};

// @class Calendar
// @brief QGraphicsScene showing week grid with event blocks.
//
// @note All coordinates are set with respect to (0,0) point located at left-top corner.
//       X increase in the right direction, while Y increase moving down.
class Calendar : public QGraphicsScene {
    Q_OBJECT
public:
    // @brief Constructor of a calendar scene.
    // @param hour_start first hour shown (0-24).
    // @param hour_end last hour shown (1-24).
    // @param parent QObject owning the scene.
    // @warning Both hour_start and hour_end must be (0-24) and hour_end must be strictly higher than hour_start.
    explicit Calendar(uint8_t hour_start = 8, uint8_t hour_end = 18, QObject *parent = nullptr);
    // @enum Location
    // @brief Constants for location on the calendar grid.
    enum class Location { kNone, kCells, kColumnHeader, kRowHeader };
    // @brief Create new event visiual based on data.
    //
    // This function adds newly created event to @ref events_ then evoke @ref select_event_groups. Then calls @ref
    // add_event_graphics on every event in order to refresh positions of every event, finally the event is add to
    // screen.
    //
    // @param event_data information about new event.
    void add_event(Event::EventData event_data);
    // @brief Checks if QTime is present in the calendar.
    bool time_in_calendar(QTime time) const {
        return (time.hour() >= hour_start_) &&
               ((time.hour() < hour_end_) || (time.hour() == hour_end_ && time.minute() == 0));
    };

private:
    uint8_t hour_start_;
    uint8_t hour_end_;
    // Set of events present on the calendar.
    std::multiset<Event *, DereferencedLess<Event>> events_[kWeekDaysSize];
    // How many subcolumns each of day has.
    uint16_t day_column_start_[kWeekDaysSize + 1];
    // Text representing days of the week.
    QGraphicsTextItem *column_header_[kWeekDaysSize];
    // Text representing hours.
    std::vector<QGraphicsTextItem *> row_header_;
    // Layout values in px
    double get_hour_height() const { return 60; };
    double get_day_column_width() const { return 160; };
    double get_column_header_height() const { return 30; };
    double get_row_header_width() const { return 40; };
    // @brief Translate time into y dimension in pixels.
    // @param time Time which location will be returned.
    // @warning time must be in between hour_start_ and hour_end_.
    double get_time_y_dimension(QTime time) const;
    // @brief Translates y dimension to height.
    // @param position Location to translate.
    // @warning position must be inside the calendar.
    QTime get_y_time_value(double position) const;
    // @brief Translate days into x dimension in pixels.
    // @param day Day which location will be returned.
    // @warning time must be in between 0 and kWeekDaysSize.
    double get_day_column_x_dimension(double day_column) const;
    // @brief Simple wrapper of @ref get_day_column_x_dimension() for full days.
    double get_day_x_dimension(uint8_t day) const { return get_day_column_x_dimension(day_column_start_[day]); };
    // @brief Translates x dimension to day.
    // @param position Location to translate.
    // @warning position must be inside the calendar.
    uint8_t get_x_day_value(double position) const;
    // @brief Get to what lacation is the point corresponding.
    //
    // Returns kNone if the point is outside the calendar or if point is in the top left corner which is neither a row
    // nor column header.
    //
    // @param point The location of the point which will be identified.
    Location identify_location(QPointF point) const;
    // @brief Divides events inside a given day of the week in the most optimal groups.
    //
    // Use greedy algorithm to assign events in a way which minimalize the number of groups.
    // It take information about the events from the @ref events_ at the respected weekday.
    //
    // @param week_day Day of the week from which events will be drawn.
    std::vector<std::vector<Event *>> select_event_groups(uint8_t week_day);
    // @brief Update the value of calendar in respect to change in one day width change.
    //
    // Perform changes only if the value of day_column_start_ needs adjustmen.
    // Updates day_column_start_ and events corresponding to every day after the week_day.
    // Update the @ref QRectF of the calendar scene to fit all day columns.
    //
    // @param week_day Day of which size is changed.
    // @param new_size New size for the day.
    // @warning week_day must be in [0, kWeekDaysSize].
    void adjust_day_column_size(uint8_t week_day, uint16_t new_size);
    // @brief Helper function for creating @ref QRectF() for an event.
    //
    // Based on the event_group and @ref groups_sizes defines the size of @ref QrectF() on which Event will be based and
    // its place.
    //
    // @param event Event which visiuals will be set.
    // @param event_group Number of group in which the event is placed.
    void add_event_graphics(Event *event, uint8_t event_group);

protected:
    // @brief Draw grid, text for hours and days.
    //
    // Implementation of a function which must be defined by a proper QGraphicsScene.
    // This function is called by Qt each time the background is refreshed.
    //
    // @param painter Pointer which will paint the rectangel.
    // @param rectangle Rectangle representing the scene.
    //
    // @sa QGraphicsScene::drawBackground()
    void drawBackground(QPainter *painter, const QRectF &rectangle) override;
    // @brief Declares behavior after the mouse click.
    //
    // Implementation of a function from QGraphicsScene.
    // Called by Qt each time the user presses on the calendar.
    //
    // @param event Information about the click.
    //
    // @sa QGraphicsScene::mousePressEvent()
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};
