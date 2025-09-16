#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QTime>
#include <QTimeEdit>

struct Event {
    QString title;
    uint8_t week_day;
    QTime start;
    QTime end;
    mutable uint8_t position = 0;
    // Comparator
    bool operator<(const Event &other) const {
        return (start < other.start) || ((start == other.start) && (end < other.end)) ||
               ((start == other.start) && (end == other.end) && (title < other.title));
    }
};

const QString kDialogTitle = "Event Creator";
const QString kDefaultEventTitle = "New Event";
const QString kTitleRow = "Title: ";
const QString kDayRowName = "Day: ";
const QString kStartTimeRowName = "Starting time: ";
const QString kEndTimeRowName = "End time: ";

class EventCreator : public QDialog {
    Q_OBJECT

public:
    explicit EventCreator(uint8_t week_day, QTime start_time, QWidget *parent = nullptr);
    Event get_data();

private:
    QLineEdit *title_box_;
    QComboBox *day_box_;
    QTimeEdit *start_time_box_;
    QTimeEdit *end_time_box_;
};
