#pragma once

#include <QAbstractTableModel>
#include <QString>

class Calendar : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QVector<QVector<QString>> data_;
    QVector<QString> column_header_;
    QVector<QString> time_column_;
};
