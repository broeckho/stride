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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Payload for Subject/Observer for myhayaia::events.
 */

#include "Id.hpp"
#include "TestResult.hpp"

#include <string>

namespace myhayai {
namespace event {

struct Payload
{
        /// Only provide event id (a.o. begin and end of benchmarking).
        explicit Payload(Id id) : m_id(id), m_test_name(), m_msg(), m_run_times(){};

        /// Provide event id, name and possibly results (executed and disabled tests).
        Payload(Id id, std::string testName, myhayai::TestResult testResult = TestResult())
            : m_id(id), m_test_name(std::move(testName)), m_msg(), m_run_times(std::move(testResult)){};

        /// Provide event id, name and message (aborted tests)
        Payload(Id id, std::string testName, std::string msg)
            : m_id(id), m_test_name(std::move(testName)), m_msg(std::move(msg)), m_run_times(){};

        Id                  m_id;
        std::string         m_test_name;
        std::string         m_msg;
        myhayai::TestResult m_run_times;
};

} // namespace event
} // namespace myhayai
