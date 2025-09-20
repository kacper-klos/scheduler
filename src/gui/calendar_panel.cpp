
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
    auto *controls_widget = new QWidget(this);
    auto *create_button = new QPushButton(kCreateButtonText, controls_widget);
    connect(create_button, &QPushButton::clicked, this, [this] {
        add_calendar_data(create_default_calendar());
    }); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
    auto *delete_button = new QPushButton(kDeleteButtonText, controls_widget);
    connect(delete_button, &QPushButton::clicked, this, &CalendarPanel::remove_calendar_data);
    delete_button->setStyleSheet("background-color: red; color: white;");
    // Layout of controls
    auto *controls_layout = new QVBoxLayout;
    controls_layout->addWidget(calendar_selector_);
    controls_layout->addWidget(create_button);
    controls_layout->addWidget(delete_button);
    controls_layout->addStretch();
    // Set layout
    controls_widget->setLayout(controls_layout);
    // Calendar graphics add do combo
    add_calendar_data(create_default_calendar()); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
    // Combined layout
    auto *full_layout = new QHBoxLayout(this); // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
    full_layout->addWidget(controls_widget);
    full_layout->addWidget(calendar_view_, 1);
}

void CalendarPanel::add_calendar_data(Calendar *calendar, QString title) {
    calendar_selector_->addItem(title, QVariant::fromValue(calendar));
    // Set to new calendar
    calendar_selector_->setCurrentIndex(calendar_selector_->count() - 1);
}

void CalendarPanel::remove_calendar_data() {
    assert(calendar_selector_->count() > 1);
    Calendar *scene_to_remove = calendar_selector_->currentData().value<Calendar *>();
    // Delete the sceene after if was switched from view by changed index signal.
    calendar_selector_->removeItem(calendar_selector_->currentIndex());
    scene_to_remove->deleteLater();
}
