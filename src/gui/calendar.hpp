#pragma once

#include <QAbstractTableModel>
#include <QString>
#include <QTime>

struct Event {
    QString title;
    QTime start;
    QTime end;
    int week_day;
};

class Calendar : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<QVector<QString>> data_;
    QVector<QString> header_column_;
    QVector<QString> header_row_;
};
