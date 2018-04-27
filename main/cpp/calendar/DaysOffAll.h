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
 * No  days off work or school.
 */
class DaysOffAll : public DaysOffInterface
{
public:
        /// Initialize calendar.
        explicit DaysOffAll(std::shared_ptr<Calendar> cal) {}

        /// See DaysOffInterface.
        bool IsWorkOff() override { return true; }

        /// See DaysOffInterface.
        bool IsSchoolOff() override { return true; }
};

} // namespace stride
