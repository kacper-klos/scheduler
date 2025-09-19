// @file const.hpp
// @brief Helper file with constants and helper functions.

#ifndef CONST_HPP_
#define CONST_HPP_

#include <QStringList>

// How many days will be considered.
inline constexpr uint8_t kWeekDaysSize = 7;
// Names of the days of the week.
inline const QStringList kWeekDays = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
// Comparator which uses defeferenced value first.
template <class T> struct DereferencedLess {
    bool operator()(const T *a, const T *b) const { return (*a < *b) || (!(*a > *b) && a < b); }
};

#endif
