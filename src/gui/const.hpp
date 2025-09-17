#pragma once

#include <QStringList>
#include <compare>

constexpr uint8_t kWeekDaysSize = 7;
const QStringList kWeekDays = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

template <class T> struct DereferencedLess {
    bool operator()(const T *a, const T *b) const { return (*a < *b) || (!(*a > *b) && a < b); }
};
