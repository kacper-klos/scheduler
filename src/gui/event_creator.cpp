#include "event_creator.hpp"
#include "const.hpp"
#include <QDialogButtonBox>
#include <QPushButton>

EventCreator::EventCreator(uint8_t week_day, QTime start_time, QWidget *parent) : QDialog(parent) {
    QDialog::setWindowTitle(kDialogTitleCreate);
    // Use default setup
    this->input_boxes_setup();
    // Day of the week selection.
    day_box_->setCurrentIndex(week_day);
    // Start time.
    start_time_box_->setTime(start_time);
    // Default event time set to one hour.
    end_time_box_->setTime(
        start_time.addSecs(kDefaultTimeSpacing.hour() * 60 * 60 + kDefaultTimeSpacing.minute() * 60));
}

EventCreator::EventCreator(Event *event, QWidget *parent) : QDialog(parent) {
    QDialog::setWindowTitle(kDialogTitleCreate);
    Event::EventData event_data = event->get_event_data();
    // Use default setup
    this->input_boxes_setup();
    // Title
    title_box_->setText(event_data.title);
    // Day of the week selection.
    day_box_->setCurrentIndex(event_data.week_day);
    // Start time.
    start_time_box_->setTime(event_data.start);
    // Default event time set to one hour.
    end_time_box_->setTime(event_data.end);
    // Delete button
    QPushButton *delete_button = new QPushButton(this);
    delete_button->setStyleSheet("background-color: red; color: white;");
    delete_button->setText(kDeleteButtonText);
    connect(delete_button, &QPushButton::clicked, this, [this] { delete_ = true; });
    connect(delete_button, &QPushButton::clicked, this, &QDialog::accept);
    layout_->addRow(delete_button);
}

void EventCreator::input_boxes_setup() {
    // Format boxes
    title_box_->setPlaceholderText(kDefaultEventTitle);
    day_box_->addItems(kWeekDays.mid(0, kWeekDaysSize));
    start_time_box_->setDisplayFormat("HH:mm");
    end_time_box_->setDisplayFormat("HH:mm");
    // Accept button.
    connect(buttons_, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons_, &QDialogButtonBox::rejected, this, &QDialog::reject);
    // Layout.
    layout_->addRow(kTitleRow, title_box_);
    layout_->addRow(kDayRowName, day_box_);
    layout_->addRow(kStartTimeRowName, start_time_box_);
    layout_->addRow(kEndTimeRowName, end_time_box_);
    layout_->addRow(buttons_);
}

Event::EventData EventCreator::get_data() const {
    QString event_title = title_box_->text().isEmpty() ? kDefaultEventTitle : title_box_->text();
    // Extract inputted data
    Event::EventData event{event_title, static_cast<uint8_t>(day_box_->currentIndex()), start_time_box_->time(),
                           end_time_box_->time()};
    return event;
}
