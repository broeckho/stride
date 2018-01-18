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
 * DaysOffAll class: everybody gets the day off.
 */

#include "DaysOffInterface.h"

namespace stride {

/**
 * Schools closed.
 */
class DaysOffSchool : public DaysOffInterface
{
public:
        /// Initialize calendar.
        DaysOffSchool(std::shared_ptr<Calendar> cal) : m_calendar(cal) {}

        /// See DaysOffInterface.
        bool IsWorkOff() override { return m_calendar->IsWeekend() || m_calendar->IsHoliday(); }

        /// See DaysOffInterface.
        virtual bool IsSchoolOff() override { return true; }

private:
        std::shared_ptr<Calendar> m_calendar; ///< Management of calendar.
};

} // namespace stride
