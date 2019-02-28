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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#pragma once

#include <stdexcept>
#include <string>

namespace stride {
namespace util {

/**
 * Basic exception class: needed to prevent clang-tidy warning
 * "thrown exception type is not nothrow copy constructible".
 */
class Exception : public std::exception
{
public:
        /// Straightforward constructor.
        explicit Exception(std::string msg) : m_msg(std::move(msg)) {}

        /// Return the message.
        const char* what() const noexcept override { return m_msg.c_str(); }

        /// Default constructor
        ~Exception() noexcept override = default;

private:
        std::string m_msg;
};

} // namespace util
} // namespace stride
