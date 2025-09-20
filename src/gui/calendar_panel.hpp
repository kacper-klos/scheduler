
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

    struct SceneRecord {
        mutable QString title;
        Calendar *calendar;
    };
    void add_calendar_data(SceneRecord data);
    void set_calendar_data() { calendar_view_->setScene(calendar_selector_->currentData().value<Calendar *>()); };
    void remove_calendar_data();

private:
    inline static const QString kDefaultCalendarName = "New Calendar";
    QGraphicsView *calendar_view_ = new QGraphicsView(this);
    QComboBox *calendar_selector_ = new QComboBox;
};

#endif
