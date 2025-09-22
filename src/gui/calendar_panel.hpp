// @file calendar_panel.hpp
// @brief Control panel for managing calendars.
//
// @note Clang tidy we use for precommit do not understand qt lifecycle.
// Because of that some lines are commented so it would not complain for unsafe memory.

#ifndef MAIN_WIDGET_HPP_
#define MAIN_WIDGET_HPP_

#include "calendar.hpp"
#include <QComboBox>
#include <QGraphicsView>
#include <QLineEdit>
#include <QWidget>

// Allow Calendar* to be used as a QVariant
Q_DECLARE_METATYPE(Calendar *)

// @class CalendarPanel
// @brief Widget for managing calendars.
class CalendarPanel : public QWidget {
    Q_OBJECT
public:
    // @brief Default QWidget creator.
    // @param parent Owner of the widget.
    explicit CalendarPanel(QWidget *parent = nullptr);
    // @brief Creates default calendar with predefined time.
    Calendar *create_default_calendar() { return new Calendar(8, 18, this); };
    // @brief Use for adding a new calendar to the combobox allowing for sellection.
    void add_calendar_data(Calendar *calendar, QString title = kDefaultCalendarName);
    // @brief Set the viewed calendar to one selected in the combobox.
    void set_calendar_data() { calendar_view_->setScene(calendar_selector_->currentData().value<Calendar *>()); };
    // @brief Remove calendar from the combobox and memory.
    void remove_calendar_data();

private:
    // Constants for visiuals.
    inline static const QString kDefaultCalendarName = "New Calendar";
    inline static const QString kCreateButtonText = "Create";
    inline static const QString kDeleteButtonText = "Delete";
    // View of the calendars.
    QGraphicsView *calendar_view_ = new QGraphicsView(this);
    // Selector for the calendars.
    QComboBox *calendar_selector_ = new QComboBox;
};

#endif
