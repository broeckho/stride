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
 *  Copyright 2019, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Definition of .
 */

#pragma once

#include "util/Exception.h"

#include <iostream>
#include <spdlog/logger.h>
#include <sstream>
#include <string>

namespace stride {
namespace util {

inline std::string AssertMessage(const char* condition, const std::string& message,
                                 const std::shared_ptr<spdlog::logger>& logger, const char* file, int line)
{
        std::ostringstream os;
        os << "Assert: '" << condition << "'"
           << " fails in: '" << file << "' line: " << line << " with: " << message;
        const auto msg = os.str();
        if (logger) {
                logger->critical(msg);
                logger->flush();
        } else {
                std::cerr << msg << std::endl;
        }
        return msg;
}

} // namespace util
} // namespace stride

// Assert that CONDITION evaluates to true, if not produce failure info string
// including file and line of the failure and its MESSAGE and
// then (for AssertAndLog) log with LOGGER or with std::cerr (iff LOOGER==nullptr),
// or (for AssertAndThrow) raise an Exception with infor string as content.
#ifdef STRIDE_INCLUDE_STRIDE_ASSERTS
#define AssertLog(CONDITION, MESSAGE, LOGGER)                                                                          \
        if (!(CONDITION)) {                                                                                            \
                stride::util::AssertMessage(#CONDITION, MESSAGE, LOGGER, __FILE__, __LINE__);                          \
        }
#define AssertThrow(CONDITION, MESSAGE, LOGGER)                                                                        \
        if (!(CONDITION)) {                                                                                            \
                throw stride::util::Exception(                                                                         \
                    stride::util::AssertMessage(#CONDITION, MESSAGE, LOGGER, __FILE__, __LINE__));                     \
        }
#else
#define AssertLog(CONDITION, MESSAGE, LOGGER) ((void)0)
#define AssertThrow(CONDITION, MESSAGE, LOGGER) ((void)0)
#endif
