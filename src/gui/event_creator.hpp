// @file event_creator.hpp
// @brief User interface for creating new Events.

#ifndef EVENT_CREATOR_HPP_
#define EVENT_CREATOR_HPP_

#include "event.hpp"
#include <QComboBox>
#include <QDialog>
#include <QTimeEdit>

// @class EventCreator
// @brief Widget from which inputted data is used to creat an Event.
class EventCreator : public QDialog {
    Q_OBJECT

public:
    // @brief Constructor of widget used to create an event.
    // @param week_day Day of the week will be selected at the start.
    // @param start_time Initial time shown in start_time_box_.
    // @param parent Object which takes ownership of the widget.
    explicit EventCreator(uint8_t week_day, QTime start_time, QWidget *parent = nullptr);
    // @brief Get data required to create new event.
    //
    // Constructs the data from all the QTimeEdit, QComboBox, QLineEdit.
    Event::EventData get_data();

private:
    // Constants
    inline static const QString kDialogTitle = "Event Creator";
    inline static const QString kDefaultEventTitle = "New Event";
    inline static const QString kTitleRow = "Title: ";
    inline static const QString kDayRowName = "Day: ";
    inline static const QString kStartTimeRowName = "Starting time: ";
    inline static const QString kEndTimeRowName = "End time: ";
    inline static const QStringList kWeekDays = {"Monday", "Tuesday",  "Wednesday", "Thursday",
                                                 "Friday", "Saturday", "Sunday"};
    inline static const QTime kDefaultTimeSpacing = QTime(1, 0);
    // Boxes of the widget which take the input.
    QLineEdit *title_box_;
    QComboBox *day_box_;
    QTimeEdit *start_time_box_;
    QTimeEdit *end_time_box_;
};

#endif
