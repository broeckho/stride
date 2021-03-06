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
 * Header file for TestDescriptors container.
 */

#pragma once

#include "TestDescriptor.h"

#include <map>
#include <string>

namespace myhayai {

/**
 * Container for the test descriptors.
 */
class TestDescriptors : public std::map<std::string, TestDescriptor>
{
public:
        /// Use the map constructors.
        using std::map<std::string, TestDescriptor>::map;

        /// Return number of tests that are disabled.
        std::size_t CountDisabled();
};

} // namespace myhayai
