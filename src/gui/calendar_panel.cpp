
#include "calendar_panel.hpp"
#include <QComboBox>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

CalendarPanel::CalendarPanel(QWidget *parent) : QWidget(parent) {
    qRegisterMetaType<Calendar *>("Calendar*");
    calendar_selector_->setInsertPolicy(QComboBox::InsertAtCurrent);
    calendar_selector_->setEditable(true);
    // Connecting change of scene
    connect(calendar_selector_, &QComboBox::currentIndexChanged, this, &CalendarPanel::set_calendar_data);
    // Left panel
    auto *button = new QPushButton;

    auto *controls_layout = new QVBoxLayout;
    controls_layout->addWidget(calendar_selector_);
    controls_layout->addWidget(button);
    controls_layout->addStretch();

    auto *controls_widget = new QWidget(this);
    controls_widget->setLayout(controls_layout);
    // Calendar graphics add do combo
    calendar_selector_->addItem( // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
        kDefaultCalendarName,
        QVariant::fromValue(new Calendar(8, 18, this))); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
    // Combined layout
    auto *full_layout = new QHBoxLayout(this);
    full_layout->addWidget(controls_widget);
    full_layout->addWidget(calendar_view_, 1);
}

void CalendarPanel::add_calendar_data(CalendarPanel::SceneRecord data) {
    calendar_selector_->addItem(data.title, QVariant::fromValue(data.calendar));
}

void CalendarPanel::remove_calendar_data() {
    assert(calendar_selector_->count() > 1);
    Calendar *scene_to_remove = calendar_selector_->currentData().value<Calendar *>();
    // Delete the sceene after if was switched from view by changed index signal.
    calendar_selector_->removeItem(calendar_selector_->currentIndex());
    scene_to_remove->deleteLater();
}
