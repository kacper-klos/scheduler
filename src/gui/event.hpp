// @file event.hpp
// @brief Graphics and logic for events put on the calendar

#ifndef EVENT_HPP_
#define EVENT_HPP_

#include <QGraphicsObject>
#include <QTime>

class Calendar;

// @class Event
// @brief Graphical representation of a single event in Calendar class.
class Event : public QGraphicsObject {
    Q_OBJECT
    friend Calendar;
signals:
    // @brief Signal showing that the event is being edited.
    void edit(Event *event);

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
    // @brief Getter for group
    uint8_t get_group() const { return group_; };

private:
    // @brief Set new rectangle and position of the Event.
    // @param rectangle New visiual base of Event.
    // @param position Pass to owner for a new location.
    void set_rectangle(QRectF rectangle, QPointF position);
    // Information about the event.
    EventData event_data_;
    uint8_t group_;
    // Base rectangle with Event visiuals.
    QRectF rectangle_ = QRectF();
    // Texts shown on the Event visiuals.
    QGraphicsTextItem *title_text_ = nullptr;
    QGraphicsTextItem *time_text_ = nullptr;

protected:
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
