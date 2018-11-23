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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#pragma once

#include <string>

namespace calibration {
/// The data that describes a boxplot
struct BoxplotData
{
        BoxplotData() : name(), min_value(), max_value(), median(), lower_quartile(), upper_quartile() {}

        std::string name;
        double      min_value;
        double      max_value;
        double      median;
        double      lower_quartile;
        double      upper_quartile;
};
} // namespace calibration
