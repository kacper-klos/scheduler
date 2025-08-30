#include "calendar.hpp"
#include "const.hpp"
#include <assert.h>

Calendar::Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent) : QAbstractTableModel(parent) {
    assert(hour_end >= hour_start && hour_end <= 24 && hour_start <= 24);

    int rows_count = (hour_end - hour_start + 1) + 1;

    QVector<QVector<QString>> data(rows_count);
    QVector<QString> header = {QString("Godzina"), QString("Ponidziałek"), QString("Wtorek"),
                               QString("Środa"),   QString("Czwartek"),    QString("Piątek")};
    data[0] = header;

    for (int i = 1; i < rows_count; ++i) {
        QVector<QString> row(WORK_DAYS_IN_WEEK_AND_HEADER);
        row[0] = QString::fromStdString(std::to_string(hour_start - 1 + i) + ":00");
        data[i] = row;
    }
    this->data_ = data;
}

int Calendar::rowCount(const QModelIndex &parent) const { return static_cast<int>(data_.size()); }

int Calendar::columnCount(const QModelIndex &parent) const { return WORK_DAYS_IN_WEEK_AND_HEADER; }

QVariant Calendar::data(const QModelIndex &index, int role) const {
    assert(index.isValid() && index.row() < this->rowCount() && index.column() < this->columnCount());
    return this->data_[index.row()][index.column()];
}

QVariant Calendar::headerData(int section, Qt::Orientation orientation, int role) const {
    if (Qt::Vertical) {
        assert(section < this->rowCount());
        return this->data_[section];
    } else {
        assert(section < this->columnCount());
        int row_count = this->rowCount();
        QList<QString> column(row_count);
        for (int i = 0; i < row_count; ++i) {
            column[i] = data_[i][section];
        }
        return column;
    }
}
