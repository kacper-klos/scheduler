#pragma once

#include <QGraphicsScene>
#include <QPainter>
#include <QString>

class Calendar : public QGraphicsScene {
    Q_OBJECT
public:
    explicit Calendar(uint8_t hour_start = 8, uint8_t hour_end = 18, QObject *parent = nullptr);

private:
    // Size in px
    double hour_height_ = 60;
    double day_width_ = 160;
    double column_header_height = 20;
    double row_header_width = 30;
    uint8_t hour_start_;
    uint8_t hour_end_;
    uint8_t hour_blocks_;

protected:
    void drawBackground(QPainter *p, const QRectF &r) override;
    void setSceneRect();
};
