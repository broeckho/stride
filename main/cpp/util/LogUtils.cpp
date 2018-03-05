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
 * Implementation of file utils.
 */

#include "LogUtils.h"

#include <iostream>
#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

using namespace spdlog;
using namespace spdlog::sinks;
using namespace std;

namespace stride {
namespace util {

std::shared_ptr<spdlog::logger> LogUtils::GetCliLogger(const string& logger_name, const string& file_name)
{
        using namespace spdlog;

        set_async_mode(1048576);
        std::shared_ptr<spdlog::logger> stride_logger = nullptr;
        try {
                vector<sink_ptr> sinks;
                const auto       color_sink = make_shared<sinks::ansicolor_stdout_sink_st>();
                sinks.push_back(color_sink);
                sinks.push_back(make_shared<sinks::simple_file_sink_st>(file_name.c_str()));
                stride_logger = make_shared<logger>(logger_name, begin(sinks), end(sinks));
                register_logger(stride_logger);
        } catch (const spdlog_ex& e) {
                cerr << "LogUtils::Setup> Stride logger initialization failed: " << e.what() << endl;
                throw;
        }
        return stride_logger;
}

std::shared_ptr<spdlog::logger> LogUtils::GetNullLogger(const string& logger_name)
{
        using namespace spdlog;
        ;
        set_async_mode(1048576);
        std::shared_ptr<spdlog::logger> stride_logger = nullptr;
        try {
                const auto null_sink = make_shared<sinks::null_sink_st>();
                stride_logger        = make_shared<logger>(logger_name, null_sink);
                register_logger(stride_logger);
        } catch (const spdlog_ex& e) {
                cerr << "LogUtils::GetNullLogger> null logger initialization failed: " << e.what() << endl;
                throw;
        }
        return stride_logger;
}

} // namespace util
} // namespace stride
