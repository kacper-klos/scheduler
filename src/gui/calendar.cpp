#include "calendar.hpp"
#include "const.hpp"
#include <assert.h>

Calendar::Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent) : QAbstractTableModel(parent) {
    assert(hour_end >= hour_start && hour_end <= 24 && hour_start <= 24);

    int rows_count = (hour_end - hour_start + 1);

    // Column header
    this->column_header_ = {QString("Godzina"), QString("Ponidziałek"), QString("Wtorek"),
                            QString("Środa"),   QString("Czwartek"),    QString("Piątek")};

    // Column with time
    QVector<QString> time_column(rows_count);
    for (int i = 0; i < rows_count; ++i) {
        time_column[i] = QString::fromStdString(std::to_string(hour_start + i) + ":00");
    }
    this->time_column_ = time_column;

    // Data vectors
    QVector<QVector<QString>> data(WORK_DAYS_IN_WEEK);
    for (int i = 0; i < WORK_DAYS_IN_WEEK; ++i) {
        QVector<QString> column(rows_count);
        data[i] = column;
    }

    this->data_ = data;
}

int Calendar::rowCount(const QModelIndex &parent) const { return static_cast<int>(this->time_column_.size()) + 1; }

int Calendar::columnCount(const QModelIndex &parent) const { return WORK_DAYS_IN_WEEK_AND_HEADER; }

QVariant Calendar::data(const QModelIndex &index, int role) const {
    assert(index.isValid() && index.row() < this->rowCount() && index.column() < this->columnCount());
    if (role == Qt::DisplayRole) {
        // Column header
        if (index.row() == 0) {
            return this->column_header_[index.column()];
        }
        // Time row
        else if (index.column() == 0) {
            return this->time_column_[index.row() - 1];
        }
        // Data
        else {
            return this->data_[index.column() - 1][index.row() - 1];
        }
    }

    return {};
}
