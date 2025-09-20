
#ifndef MAIN_WIDGET_HPP_
#define MAIN_WIDGET_HPP_

#include "calendar.hpp"
#include <QComboBox>
#include <QGraphicsView>
#include <QLineEdit>
#include <QWidget>

Q_DECLARE_METATYPE(Calendar *)

class CalendarPanel : public QWidget {
    Q_OBJECT
public:
    explicit CalendarPanel(QWidget *parent = nullptr);

    Calendar *create_default_calendar() { return new Calendar(8, 18, this); };
    void add_calendar_data(Calendar *calendar, QString title = kDefaultCalendarName);
    void set_calendar_data() { calendar_view_->setScene(calendar_selector_->currentData().value<Calendar *>()); };
    void remove_calendar_data();

private:
    inline static const QString kDefaultCalendarName = "New Calendar";
    inline static const QString kCreateButtonText = "Create";
    inline static const QString kDeleteButtonText = "Delete";
    QGraphicsView *calendar_view_ = new QGraphicsView(this);
    QComboBox *calendar_selector_ = new QComboBox;
};

#endif
