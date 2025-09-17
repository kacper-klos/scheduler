#pragma once

#include <QComboBox>
#include <QDialog>
#include <QLineEdit>
#include <QTime>
#include <QTimeEdit>
#include <compare>
#include <tuple>

struct EventData {
    QString title;
    uint8_t week_day;
    QTime start;
    QTime end;
    // Comparator
    auto key() const {
        return std::make_tuple(start.hour(), start.minute(), end.hour(), end.minute(), title.toStdString(), week_day);
    }
    std::strong_ordering operator<=>(const EventData &other) const { return key() <=> other.key(); }
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
    EventData get_data();

private:
    QLineEdit *title_box_;
    QComboBox *day_box_;
    QTimeEdit *start_time_box_;
    QTimeEdit *end_time_box_;
};
