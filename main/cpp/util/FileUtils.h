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
 * Header file for file utils
 */

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace stride {
namespace util {

/**
 * Completes the file relative to root.
 * @param filename: filename to complete
 * @param root: default execution path.
 * @return the full path to the file if it exists
 */
const boost::filesystem::path completePath(const boost::filesystem::path& filename,
                                           const boost::filesystem::path& root = boost::filesystem::current_path());

/**
 * Checks if there is a file with "filename" relative to the exectution path.
 * @param filename: filename to check.
 * @param root: Default execution path.
 * @return the full path to the file if it exists
 * @throws runtime error if file doesn't exist
 */
const boost::filesystem::path checkFile(const boost::filesystem::path& filename,
                                        const boost::filesystem::path& root = boost::filesystem::current_path());

} // namespace util
} // namespace stride
