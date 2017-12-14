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
 * Implementation of LogMode.
 */

#include "LogMode.h"

#include <boost/algorithm/string.hpp>
#include <map>

namespace {

using stride::LogMode;
using boost::to_upper;
using namespace std;

map<LogMode, string> g_log_mode_name{make_pair(LogMode::None, "None"),
				     make_pair(LogMode::Transmissions, "Transmissions"),
				     make_pair(LogMode::Contacts, "Contacts"), make_pair(LogMode::Null, "Null")};

map<string, LogMode> g_name_log_mode{make_pair("NONE", LogMode::None),
				     make_pair("TRANSMISSIONS", LogMode::Transmissions),
				     make_pair("CONTACTS", LogMode::Contacts), make_pair("NULL", LogMode::Null)};
}

namespace stride {

string ToString(LogMode l) { return (g_log_mode_name.count(l) == 1) ? g_log_mode_name[l] : "Null"; }

bool IsLogMode(const string& s)
{
	std::string t{s};
	to_upper(t);
	return (g_name_log_mode.count(t) == 1);
}

LogMode ToLogMode(const string& s)
{
	std::string t{s};
	to_upper(t);
	return (g_name_log_mode.count(t) == 1) ? g_name_log_mode[t] : LogMode::Null;
}

} // namespace
