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

using namespace spdlog;
using namespace spdlog::sinks;
using namespace std;

namespace stride {
namespace util {

std::shared_ptr<spdlog::logger> LogUtils::CreateCliLogger(const string& logger_name, const string& file_name)
{
        set_async_mode(1048576);
        auto lggr = get(logger_name);
        if (lggr) {
                throw runtime_error("LogUtils::CreateCliLogger> Creating already registered logger" + logger_name);
        }
        try {
                vector<sink_ptr> sinks;
                const auto       color_sink = make_shared<ansicolor_stdout_sink_st>();
                sinks.push_back(color_sink);
                sinks.push_back(make_shared<simple_file_sink_st>(file_name));
                lggr = make_shared<logger>(logger_name, begin(sinks), end(sinks));
        } catch (const spdlog_ex& e) {
                cerr << "LogUtils::CreateCliLogger> Logger initialization failed for " << logger_name
                     << " and file: " << file_name << endl;
                throw;
        }
        return lggr;
}

std::shared_ptr<spdlog::logger> LogUtils::CreateFileLogger(const string& logger_name, const string& file_name)
{
        set_async_mode(1048576);
        auto lggr = get(logger_name);
        if (lggr) {
                throw runtime_error("LogUtils::CreateFileLogger> Creating already registered logger" + logger_name);
        }
        try {
                const auto sink = make_shared<simple_file_sink_st>(file_name, true);
                lggr            = make_shared<logger>(logger_name, sink);
        } catch (const spdlog_ex& e) {
                cerr << "LogUtils::CreateFileLogger> Logger initialization failed for " << logger_name
                     << " and file: " << file_name << endl;
                throw;
        }
        return lggr;
}

std::shared_ptr<logger> LogUtils::CreateNullLogger(const string& logger_name)
{
        set_async_mode(1048576);
        auto lggr = get(logger_name);
        if (lggr) {
                throw runtime_error("LogUtils::CreateNullLogger> Creating already registered logger" + logger_name);
        }
        try {
                const auto null_sink = make_shared<null_sink_st>();
                lggr                 = make_shared<logger>(logger_name, null_sink);
        } catch (const spdlog_ex& e) {
                cerr << "LogUtils::CreateNullLogger> Logger initialization failed for " << logger_name << endl;
                throw;
        }
        return lggr;
}

std::shared_ptr<logger> LogUtils::CreateRotatingLogger(const string& logger_name, const string& file_name)
{
        set_async_mode(1048576);
        auto lggr = get(logger_name);
        if (lggr) {
                throw runtime_error("LogUtils::CreateRotatingLogger> Creating already registered logger" + logger_name);
        }
        try {
                auto rot = make_shared<rotating_file_sink_mt>(file_name, numeric_limits<size_t>::max(),
                                                              numeric_limits<size_t>::max());
                lggr     = make_shared<logger>(logger_name, rot);
        } catch (const spdlog_ex& e) {
                cerr << "LogUtils::CreateRotatingLogger> Logger initialization failed for " << logger_name
                     << " and file: " << file_name << endl;
                throw;
        }
        return lggr;
}

} // namespace util
} // namespace stride
