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
 * Logging (spdlog) utilities.
 */

#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace stride {
namespace util {

/**
 * Utilities for logging (spdlog).
 */
class LogUtils
{
public:
        ///
        /// \param logger_name
        /// \param file_name
        /// \return
        static std::shared_ptr<spdlog::logger> GetCliLogger(const std::string& logger_name,
                                                            const std::string& file_name);

        /// \param logger_name
        /// \return
        static std::shared_ptr<spdlog::logger> GetNullLogger(const std::string& logger_name);
};

} // namespace util
} // namespace stride
