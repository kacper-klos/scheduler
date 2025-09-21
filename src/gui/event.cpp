#include "event.hpp"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

Event::Event(Event::EventData &event_data, QGraphicsItem *parent) : event_data_(event_data), QGraphicsObject(parent) {
    // Initialize text
    title_text_ = new QGraphicsTextItem(this);
    time_text_ = new QGraphicsTextItem(this);
    // Limit objects the the event block size
    this->setFlag(QGraphicsItem::ItemClipsChildrenToShape, true);
    this->title_text_->setDefaultTextColor(Qt::black);
    this->time_text_->setDefaultTextColor(Qt::black);
}

void Event::set_rectangle(QRectF new_rectangle, QPointF position) {
    this->prepareGeometryChange();
    this->setPos(position);
    rectangle_ = new_rectangle;
    this->update();
}

void Event::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // Constants
    constexpr double kTextPaddingX = 2;
    constexpr double kTextPaddingY = 1;
    // Check if is valid
    QRectF rectangle = boundingRect();
    if (!rectangle.isValid()) {
        return;
    }
    painter->setBrush(Qt::red);
    painter->setPen(QPen(Qt::red, 1));
    painter->drawRect(boundingRect());
    double width = this->boundingRect().width();
    // Writes text in event
    title_text_->setFont(QFont("Inter", static_cast<int>(width * 0.08)));
    title_text_->setPlainText(event_data_.title);
    title_text_->setPos(kTextPaddingX, kTextPaddingY);
    title_text_->setTextWidth(width - 2 * kTextPaddingX);

    time_text_->setFont(QFont("Inter", static_cast<int>(width * 0.06)));
    time_text_->setPlainText(event_data_.start.toString("H:mm") + " - " + event_data_.end.toString("H:mm"));
    time_text_->setPos(kTextPaddingX, title_text_->boundingRect().height());
    time_text_->setTextWidth(width - 2 * kTextPaddingX);
}

void Event::mousePressEvent(QGraphicsSceneMouseEvent *click) {
    // Accepts only right click.
    if (click->button() != Qt::RightButton) {
        click->ignore();
        return;
    }
    emit edit(this);
    click->accept();
}
