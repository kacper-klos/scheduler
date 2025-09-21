// @file calendar.hpp
// @brief User interface for calendar and it events.
//
// Units:
// - Pixels
// Ownership:
// - Every item created by Calendar is owned by it.

#ifndef CALENDAR_HPP_
#define CALENDAR_HPP_

#include "const.hpp"
#include "event.hpp"
#include <QGraphicsScene>
#include <set>

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
    // @brief Create new event based on data.
    //
    // This function adds newly created event to @ref events_ and to the Calendar display.
    //
    // @param event_data information about new event.
    void add_event(Event::EventData event_data);
    // @brief Checks if QTime is present in the calendar.
    bool time_in_calendar(QTime time) const {
        return (time.hour() >= hour_start_) &&
               ((time.hour() < hour_end_) || (time.hour() == hour_end_ && time.minute() == 0));
    };
    inline static constexpr uint8_t kWeekDaysSize = 7;

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
    std::vector<std::vector<Event *>> select_event_groups(uint8_t week_day) const;
    // @brief Divides the day into groups.
    //
    // Select groups so no event in the group colide and the number of groups is minimal.
    // Used by @ref refresh_day_graphicly to set proper visiuals.
    //
    // @param week_day Day of which size is changed.
    // @warning week_day must be in [0, kWeekDaysSize].
    void adjust_day_column_size(uint8_t week_day, uint16_t new_size);
    // @brief Helper function for creating @ref QRectF() for an event.
    //
    // Based on the properties of @ref Event defines the size of @ref QrectF() on which Event will be based and
    // place of the Event.
    //
    // @param event Event which visiuals bounds will be set.
    void add_event_graphics(Event *event);
    // @brief Set events in a day to they proper location.
    //
    // Add events to view and sets they size using @ref add_event_graphics.
    // Updates day_column_start_ and events corresponding to every day after the week_day.
    //
    // @param week_day Day which will be refreshed.
    void refresh_day_graphicly(uint8_t week_day);
    // @brief Edit the event.
    //
    // Show widget for editing the event and take its input.
    //
    // @param event Event to edit.
    void edit_event_action(Event *event);
    // @brief Safely removes the event
    // @param event Event to remove.
    void delete_event(Event *event);

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
    // @param click Information about the click.
    //
    // @sa QGraphicsScene::mousePressEvent()
    void mousePressEvent(QGraphicsSceneMouseEvent *click) override;
};

#endif
