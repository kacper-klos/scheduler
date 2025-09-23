// @file event_creator.hpp
// @brief User interface for creating new Events.

#ifndef EVENT_CREATOR_HPP_
#define EVENT_CREATOR_HPP_

#include "event.hpp"
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
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
    // @param lower_time_limit The lowest accepted event start time.
    // @param lower_time_limit The highest accepted event end time.
    explicit EventCreator(uint8_t week_day, QTime start_time, QTime lower_time_limit, QTime upper_time_limit,
                          QWidget *parent = nullptr);
    // @brief Constructor of widget used to edit an event.
    // @param current_event Event which will be edited.
    // @param lower_time_limit The lowest accepted event start time.
    // @param lower_time_limit The highest accepted event end time.
    explicit EventCreator(Event *current_event, QTime lower_time_limit, QTime upper_time_limit,
                          QWidget *parent = nullptr);
    // @brief Get data required to create new event.
    //
    // Constructs the data from all the QTimeEdit, QComboBox, QLineEdit.
    Event::EventData get_data() const;
    // @brief Gettor of delete_.
    bool get_delete() const { return delete_; };

private:
    // Was event deleted
    bool delete_ = false;
    // Constants
    inline static const QString kDialogTitleCreate = "Event Creator";
    inline static const QString kDialogTitleEdit = "Event Edit";
    inline static const QString kDefaultEventTitle = "New Event";
    inline static const QString kTitleRow = "Title: ";
    inline static const QString kDayRowName = "Day: ";
    inline static const QString kStartTimeRowName = "Starting time: ";
    inline static const QString kEndTimeRowName = "End time: ";
    inline static const QString kDeleteButtonText = "Delete";
    inline static const QStringList kWeekDays = {"Monday", "Tuesday",  "Wednesday", "Thursday",
                                                 "Friday", "Saturday", "Sunday"};
    inline static const QTime kDefaultTimeSpacing = QTime(1, 0);
    // @brief Setup widget visuals.
    //
    // Add boxes to the widget, format them, and place them in the layout.
    //
    // @param lower_time_limit The lowest accepted event start time.
    // @param lower_time_limit The highest accepted event end time.
    void input_boxes_setup(QTime lower_time_limit, QTime upper_time_limit);
    // Boxes of the widget which take the input.
    QLineEdit *title_box_ = new QLineEdit(this);
    QComboBox *day_box_ = new QComboBox(this);
    QTimeEdit *start_time_box_ = new QTimeEdit(this);
    QTimeEdit *end_time_box_ = new QTimeEdit(this);
    QDialogButtonBox *buttons_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QFormLayout *layout_ = new QFormLayout(this);
};

#endif
