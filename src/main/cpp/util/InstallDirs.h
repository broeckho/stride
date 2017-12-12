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
 * Interface for install directory queries.
 */

#include <boost/filesystem.hpp>

namespace stride {
namespace util {

/**
 * Install directories.
 */
class InstallDirs
{
public:
	/// Utility method: get path to bin directory.
	static boost::filesystem::path GetBinDir();

	/// Utility method: get path to the current directory.
	static boost::filesystem::path GetCurrentDir();

	/// Utility method: get path to the directory for data files.
	static boost::filesystem::path GetDataDir();

	/// Utility method: get name of executable.
	static boost::filesystem::path GetExecPath();

	/// Utility method: get application installation root directory.
	static boost::filesystem::path GetRootDir();

private:
	/// Check initialization.
	static void Check();

	/// Initialize all paths.
	static void Initialize();

private:
	static boost::filesystem::path g_bin_dir;
	static boost::filesystem::path g_current_dir;
	static boost::filesystem::path g_data_dir;
	static boost::filesystem::path g_exec_path;
	static boost::filesystem::path g_root_dir;
};

} // end of namespace
} // end of namespace

