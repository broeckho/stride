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
 * Utilities for interaction with filesystem.
 */

#include "FileSys.h"

#include "util/StringUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <regex>
#include <string>

#if defined(WIN32)
#include <stdlib.h>
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

namespace {
const auto empty_path = filesys::path();
}

namespace stride {
namespace util {

using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;

filesys::path FileSys::BuildPath(const std::string& output_prefix, const std::string& filename)
{
        filesys::path p = output_prefix;
        if (FileSys::IsDirectoryString(output_prefix)) {
                // file <filename> in dircetory <output_prefix>
                p /= filename;
        } else {
                // full name is <output_prefix><filename>
                p += "_" + filename;
        }
        return p;
}

bool FileSys::CheckInstallEnv(function<void(const string&)> logger)
{
        bool status = true;

        // Current working dir has to be install root dir.
        if (GetCurrentDir().compare(GetRootDir()) != 0) {
                if (logger)
                        logger("Current working dir not install root!");
                status = false;
        }
        /// There has to be a config dir in the install root dir.
        if (GetConfigDir().empty()) {
                if (logger)
                        logger("Config dir not present in install root!");
                status = false;
        }
        /// There has to be a data dir in the install root dir.
        if (GetDataDir().empty()) {
                if (logger)
                        logger("Data dir not present in install root!");
                status = false;
        }
        /// There has to be a data dir in the install root dir.
        if (GetTestsDir().empty()) {
                if (logger)
                        logger("Tests dir not present in install root!");
                status = false;
        }
        return status;
}

FileSys::Dirs& FileSys::Get()
{
        static Dirs dirs = Initialize();
        return dirs;
}

FileSys::Dirs FileSys::Initialize()
{
        Dirs dirs;
        //------- Retrieving path of executable
        {
#if defined(WIN32)
                char    exePath[MAX_PATH];
                HMODULE hModule = GetModuleHandle(NULL);
                if (GetModuleFileName(NULL, exePath, sizeof(exePath)) != 0)
                        ;
                {
                        dirs.m_exec_path = canonical(filesys::absolute(exePath));
                }
#elif defined(__linux__)
                char exePath[PATH_MAX];
                auto size = static_cast<std::size_t>(::readlink("/proc/self/exe", exePath, sizeof(exePath)));
                if (size > 0 && size < sizeof(exePath)) {
                        exePath[size]    = '\0';
                        dirs.m_exec_path = canonical(filesys::absolute(exePath));
                }
#elif defined(__APPLE__)
                char     exePath[PATH_MAX];
                uint32_t size = sizeof(exePath);
                if (_NSGetExecutablePath(exePath, &size) == 0) {
                        dirs.m_exec_path = canonical(filesys::absolute(exePath));
                }
#endif
        }

        //------- Retrieving root and bin directory (the subdirectory of the install root)
        {
                filesys::path exec_dir = dirs.m_exec_path.parent_path();
                if (!dirs.m_exec_path.empty()) {
#if (__APPLE__)
                        if (exec_dir.filename().string() == "MacOS") {
                                // app
                                //      -Contents               <-Root Path
                                //              -MacOS
                                //                   -executables
                                dirs.m_bin_dir  = exec_dir;
                                dirs.m_root_dir = exec_dir.parent_path();
                        } else
#endif
                            if (ToLower(exec_dir.filename().string()) == "debug" ||
                                ToLower(exec_dir.filename().string()) == "release") {
                                // x/exec                <-Root Path
                                //      -bin
                                //              -release/debug
                                //                      -executables
                                dirs.m_bin_dir  = exec_dir.parent_path();
                                dirs.m_root_dir = exec_dir.parent_path().parent_path();
                        } else
#if (WIN32)
                            if (exec_dir.filename().string() != "bin") {
                                // Executables in root folder
                                dirs.m_bin_dir  = exec_dir;
                                dirs.m_root_dir = exec_dir;
                        } else
#endif
                        {
                                // x/exec                <-Root Path
                                //      -bin
                                //              -executables
                                dirs.m_bin_dir  = exec_dir;
                                dirs.m_root_dir = exec_dir.parent_path();
                        }
                }
        }
        //------- Current Dir
        {
                dirs.m_current_dir = filesys::absolute(filesys::current_path());
        }
        //------- Config Dir
        {
                dirs.m_config_dir = dirs.m_root_dir / "config";
                dirs.m_config_dir = is_directory(dirs.m_config_dir) ? dirs.m_config_dir : empty_path;
        }
        //------- Data Dir
        {
                dirs.m_data_dir = dirs.m_root_dir / "data";
                dirs.m_data_dir = is_directory(dirs.m_data_dir) ? dirs.m_data_dir : empty_path;
        }
        //------- Tests Dir
        {
                dirs.m_tests_dir = dirs.m_root_dir / "tests";
                dirs.m_tests_dir = is_directory(dirs.m_tests_dir) ? dirs.m_tests_dir : empty_path;
        }

        return dirs;
}

bool FileSys::IsDirectoryString(const string& s)
{
        const auto n = s.find('/');
        return n != string::npos;
}

bool FileSys::CreateDirectory(std::string s)
{
        if (std::regex_match(s, std::regex(".*/$"))) {
                // Strip the trailing / to make it work with std::filesystem
                s = std::regex_replace(s, std::regex("\\/$"), "");
        }
        return filesys::create_directories(filesys::current_path() / s);
}

ptree FileSys::ReadPtreeFile(const filesys::path& f_p)
{
        ptree ret;
        if (!exists(f_p) || !is_regular_file(f_p)) {
                const string s = "FileSys::ReadPtreeFile> Abort! File " + f_p.string() + " not present.";
                cerr << s << endl;
                throw runtime_error(s);
        } else {
                try {
                        read_xml(canonical(f_p).string(), ret, xml_parser::trim_whitespace);
                } catch (xml_parser_error& e) {
                        const string s = "FileSys::ReadPtreeFile> Abort! Error reading " + f_p.string();
                        cerr << s << endl;
                        throw runtime_error(s);
                }
        }
        return ret;
}

ptree FileSys::ReadPtreeFile(const string& f_n) { return ReadPtreeFile(filesys::absolute(f_n)); }

void FileSys::WritePtreeFile(const filesys::path& f_p, const boost::property_tree::ptree& pt)
{
        try {
                write_xml(f_p.string(), pt, std::locale(), xml_writer_make_settings<ptree::key_type>(' ', 8));
        } catch (xml_parser_error& e) {
                const string s = "FileSys::ReadPtreeFile> Abort! Error reading " + f_p.string();
                cerr << s << endl;
                throw runtime_error(s);
        }
}

void FileSys::WritePtreeFile(const string& f_n, const boost::property_tree::ptree& pt)
{
        WritePtreeFile(filesys::absolute(f_n), pt);
}

} // namespace util
} // namespace stride
