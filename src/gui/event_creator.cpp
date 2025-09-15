#include "event_creator.hpp"
#include "const.hpp"
#include <QFormLayout>

EventCreator::EventCreator(QString title, uint8_t week_day, QTime start_time, QWidget *parent) : QDialog(parent) {

    QDialog::setWindowTitle(kDialogTitle);

    title_box_ = new QLineEdit(this);
    title_box_->setPlaceholderText(kDefaultEventTitle);

    day_box_ = new QComboBox(this);
    day_box_->addItems(kWeekDays);
    day_box_->setCurrentIndex(week_day);

    start_time_box_ = new QTimeEdit(this);
    start_time_box_->setDisplayFormat("HH:mm");
    start_time_box_->setTime(start_time);
    // Default event time set to one hour
    end_time_box_ = new QTimeEdit(this);
    end_time_box_->setDisplayFormat("HH:mm");
    end_time_box_->setTime(start_time.addSecs(60 * 60));

    auto *layout = new QFormLayout(this);
    layout->addRow(kTitleRow, title_box_);
    layout->addRow(kDayRowName, day_box_);
    layout->addRow(kStartTimeRowName, start_time_box_);
    layout->addRow(kEndTimeRowName, end_time_box_);
}

Event EventCreator::GetData() {

    Event event{title_box_->text(), static_cast<uint8_t>(day_box_->currentIndex()), start_time_box_->time(),
                end_time_box_->time()};
    return event;
}
