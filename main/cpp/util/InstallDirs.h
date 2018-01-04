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
        /// Ininitilize.
        InstallDirs();

        /// Utility method: get path to bin directory.
        boost::filesystem::path GetBinDir() const { return m_bin_dir; }

        /// Utility method: get path to the current directory.
        boost::filesystem::path GetCurrentDir() const { return m_current_dir; }

        /// Utility method: get path to the directory for data files.
        boost::filesystem::path GetDataDir() const { return m_data_dir; }

        /// Utility method: get name of executable.
        boost::filesystem::path GetExecPath() const { return m_exec_path; }

        /// Utility method: get application installation root directory.
        boost::filesystem::path GetRootDir() const { return m_root_dir; }

private:
        /// Initialize all paths.
        void Initialize();

private:
        boost::filesystem::path m_bin_dir;
        boost::filesystem::path m_current_dir;
        boost::filesystem::path m_data_dir;
        boost::filesystem::path m_exec_path;
        boost::filesystem::path m_root_dir;
};

} // end of namespace
} // end of namespace
