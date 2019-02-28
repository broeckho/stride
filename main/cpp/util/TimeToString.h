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
 * Utilities to tag clocks and reformat clock readout to string.
 */

#pragma once

#include <chrono>
#include <string>

namespace stride {
namespace util {

/**
 * Utilities to tag clocks and to reformat number of ticks to a string.
 */
struct TimeToString
{
        /// Produce string in hh:mm:ss format.
        static std::string ToColonString(std::chrono::minutes d);

        /// Produce string in hh:mm:ss format.
        static std::string ToColonString(std::chrono::seconds d);

        /// Produce string in hh:mm:ss:mus format
        static std::string ToColonString(std::chrono::milliseconds d);

        /// Produce string in hh:mm:ss:ms:mus format.
        static std::string ToColonString(std::chrono::microseconds d);

        /// Produce string in hh:mm:ss:ms:mus:ns format.
        static std::string ToColonString(std::chrono::nanoseconds d);
};

} // namespace util
} // namespace stride
