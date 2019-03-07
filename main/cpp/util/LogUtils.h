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

#pragma once

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
        /// Return a (not-yet-registered) commandline and file logger, without registering it.
        /// The file, if it already exits is truncated when opened.
        /// Throws iff logger already registered or if spdlog throws.
        static std::shared_ptr<spdlog::logger> CreateCliLogger(const std::string& logger_name,
                                                               const std::string& file_name);

        /// Return a (not-yet-registered) file logger, without registering it.
        /// The file, if it already exits is truncated when opened.
        /// Throws iff logger already registered or if spdlog throws.
        static std::shared_ptr<spdlog::logger> CreateFileLogger(const std::string& logger_name,
                                                                const std::string& file_name);
        /// Return a (not-yet-registered) null logger, without registering it.
        /// Throws iff logger already registered or if spdlog throws.
        static std::shared_ptr<spdlog::logger> CreateNullLogger(const std::string& logger_name = "null_logger");

        /// Return a (not-yet-registered) rotating logger, without registering it.
        /// Throws iff logger already registered or if spdlog throws.
        static std::shared_ptr<spdlog::logger> CreateRotatingLogger(const std::string& logger_name,
                                                                    const std::string& file_name);
};

} // namespace util
} // namespace stride
