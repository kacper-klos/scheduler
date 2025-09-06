#include "calendar.hpp"
#include <assert.h>

Calendar::Calendar(uint8_t hour_start, uint8_t hour_end, QObject *parent) : QAbstractTableModel(parent) {
    assert(hour_end >= hour_start && hour_end <= 24 && hour_start <= 24);

    int rows_count = (hour_end - hour_start + 1);

    // Column header
    this->header_column_ = {QString("Ponidziałek"), QString("Wtorek"), QString("Środa"), QString("Czwartek"),
                            QString("Piątek")};

    // Column with time
    QVector<QString> time_column(rows_count);
    for (int i = 0; i < rows_count; ++i) {
        time_column[i] = QString::fromStdString(std::to_string(hour_start + i) + ":00");
    }
    this->header_row_ = time_column;

    // Data vectors
    QVector<QVector<QString>> data(header_column_.length());
    for (int i = 0; i < header_column_.length(); ++i) {
        QVector<QString> column(rows_count);
        data[i] = column;
    }

    this->data_ = data;
}

int Calendar::rowCount(const QModelIndex &parent) const { return static_cast<int>(this->header_row_.size()); }

int Calendar::columnCount(const QModelIndex &parent) const { return header_column_.length(); }

QVariant Calendar::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole) {
        return {};
    }

    if (orientation == Qt::Horizontal) {
        assert(section < this->header_column_.length());
        return this->header_column_[section];
    } else {
        assert(section < this->header_row_.length());
        return this->header_row_[section];
    }
}

QVariant Calendar::data(const QModelIndex &index, int role) const {
    assert(index.isValid() && index.row() < this->rowCount() && index.column() < this->columnCount());
    if (role == Qt::DisplayRole) {
        return this->data_[index.column()][index.row()];
    }

    return {};
}
