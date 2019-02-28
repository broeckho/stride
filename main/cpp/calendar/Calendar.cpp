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
 * Implementation file for the Calendar class.
 */

#include "Calendar.h"

#include "util/FileSys.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace stride {

using namespace std;
using namespace boost::property_tree::json_parser;
using namespace stride::util;
using boost::property_tree::ptree;

#ifdef BOOST_FOUND

Calendar::Calendar(const ptree& configPt) : m_date(), m_holidays(), m_school_holidays(), m_day(0U)
{
        // Set start date
        m_date = boost::gregorian::from_simple_string(configPt.get<string>("run.start_date", "2016-01-01"));

        // Set holidays & school holidays
        InitializeHolidays(configPt);
}

void Calendar::AdvanceDay()
{
        m_day++;
        m_date = m_date + boost::gregorian::date_duration(1);
}

size_t Calendar::GetDay() const { return m_date.day(); }

size_t Calendar::GetDayOfTheWeek() const { return m_date.day_of_week(); }

size_t Calendar::GetMonth() const { return m_date.month(); }

unsigned short int Calendar::GetSimulationDay() const { return m_day; }

size_t Calendar::GetYear() const { return m_date.year(); }

void Calendar::InitializeHolidays(const ptree& configPt)
{
        // Load json file
        ptree holidaysPt;
        {
                const string        fName{configPt.get<string>("run.holidays_file", "holidays_flanders_2017.json")};
                const filesys::path fPath{FileSys::GetDataDir() /= fName};
                if (!is_regular_file(fPath)) {
                        throw runtime_error(string(__func__) + "Holidays file " + fPath.string() + " not present.");
                }
                read_json(fPath.string(), holidaysPt);
        }

        // Read in holidays
        for (int i = 1; i < 13; i++) {
                const auto month = to_string(i);
                const auto year  = holidaysPt.get<string>("year", "2017");
                const auto lead  = string(year).append("-").append(month).append("-");

                // read in general holidays
                for (const auto& date : holidaysPt.get_child("general." + month)) {
                        const auto d = string(lead).append(date.second.get_value<string>());
                        m_holidays.push_back(boost::gregorian::from_simple_string(d));
                }

                // read in school holidays
                for (const auto& date : holidaysPt.get_child("school." + month)) {
                        const string d = string(lead).append(date.second.get_value<string>());
                        m_school_holidays.push_back(boost::gregorian::from_simple_string(d));
                }
        }
}

#else

date::year_month_day ConvertFromString(const string& day)
{
        tm           timeinfo{};
        stringstream ss(day);
        ss >> get_time(&timeinfo, "%Y-%m-%d");
        auto date = date::year{timeinfo.tm_year + 1900} / date::month{static_cast<unsigned int>(timeinfo.tm_mon + 1)} /
                    date::day{static_cast<unsigned int>(timeinfo.tm_mday)};
        return date;
}

Calendar::Calendar(const boost::property_tree::ptree& configPt)
    : m_date(), m_holidays(), m_school_holidays(), m_day(static_cast<size_t>(0))
{
        const string start_date{configPt.get<string>("run.start_date", "2016-01-01")};
        // Set start date
        m_date = ConvertFromString(start_date);
        // Set holidays & school holidays
        InitializeHolidays(configPt);
}

void Calendar::AdvanceDay()
{
        m_day++;
        m_date = static_cast<date::year_month_day>(static_cast<date::sys_days>(m_date) + date::days(1));
}

void Calendar::InitializeHolidays(const ptree& configPt)
{
        // Load json file
        ptree holidaysPt;
        {
                const string           fName{configPt.get<string>("run.holidays_file", "holidays_flanders_2017.json")};
                const filesystem::path fPath{FileSys::GetDataDir() /= fName};
                if (!is_regular_file(fPath)) {
                        throw runtime_error(string(__func__) + "Holidays file " + fPath.string() + " not present.");
                }
                read_json(fPath.string(), holidaysPt);
        }

        // Read in holidays
        for (int i = 1; i < 13; i++) {
                const auto month = to_string(i);
                const auto year  = holidaysPt.get<string>("year", "2017");

                // read in general holidays
                for (const auto& date : holidaysPt.get_child("general." + month)) {
                        stringstream d;
                        /// Append zero's due to a bug in stdc++ https://gcc.gnu.org/bugzilla/show_bug.cgi?id=45896
                        d << year << "-" << setw(2) << setfill('0') << month << "-" << setw(2) << setfill('0')
                          << date.second.get_value<string>();
                        m_holidays.push_back(ConvertFromString(d.str()));
                }

                // read in school holidays
                for (const auto& date : holidaysPt.get_child("school." + month)) {
                        stringstream d;
                        /// Append zero's due to a bug in stdc++ https://gcc.gnu.org/bugzilla/show_bug.cgi?id=45896
                        d << year << "-" << setw(2) << setfill('0') << month << "-" << setw(2) << setfill('0')
                          << date.second.get_value<string>();
                        m_school_holidays.push_back(ConvertFromString(d.str()));
                }
        }
}

size_t Calendar::GetDay() const { return static_cast<unsigned int>(m_date.day()); }

size_t Calendar::GetDayOfTheWeek() const
{
        return static_cast<unsigned>(static_cast<date::year_month_weekday>(m_date).weekday());
}

size_t Calendar::GetMonth() const { return static_cast<unsigned int>(m_date.month()); }

unsigned short int Calendar::GetSimulationDay() const { return m_day; }

size_t Calendar::GetYear() const { return static_cast<size_t>(static_cast<int>(m_date.year())); }

#endif

} // namespace stride
