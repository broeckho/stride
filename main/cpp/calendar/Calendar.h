#pragma once
/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for the Calendar class.
 */

#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/property_tree/ptree.hpp>

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>

namespace stride {

/**
 * Class that keeps track of the 'state' of simulated world.
 * E.g. what day it is, holidays, quarantines, ...
 */
class Calendar
{
public:
        /// Constructor
        explicit Calendar(const boost::property_tree::ptree& pt_config);

        /// Advance the internal calendar by one day
        void AdvanceDay();

        /// Get the current day of the month
        std::size_t GetDay() const { return m_date.day(); }

        /// Get the current day of the week (0 (Sunday), ..., 6 (Saturday))
        std::size_t GetDayOfTheWeek() const { return m_date.day_of_week(); }

        /// Get the current month
        std::size_t GetMonth() const { return m_date.month(); }

        /// Get the current day of the simulation
        std::size_t GetSimulationDay() const { return m_day; }

        /// Get the current year
        std::size_t GetYear() const { return m_date.year(); }

        /// Check if it's a holiday
        bool IsHoliday() const { return (std::find(m_holidays.begin(), m_holidays.end(), m_date) != m_holidays.end()); }

        /// Check if it's a school holiday
        bool IsSchoolHoliday() const
        {
                return (std::find(m_school_holidays.begin(), m_school_holidays.end(), m_date) !=
                        m_school_holidays.end());
        }

        /// Check if it's the weekend
        bool IsWeekend() const { return (GetDayOfTheWeek() == 6 || GetDayOfTheWeek() == 0); }

private:
        ///
        void InitializeHolidays(const boost::property_tree::ptree& pt_config);

private:
        std::size_t                         m_day;             ///< The current simulation day
        boost::gregorian::date              m_date;            ///< The current simulated day
        std::vector<boost::gregorian::date> m_holidays;        ///< Vector of general holidays
        std::vector<boost::gregorian::date> m_school_holidays; ///< Vector of school holidays
};

} // namespace stride
