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
 * Produce run config ptree.
 */

#include <string>

namespace stride {
namespace util {

/**
 * Produce run config xml string. Intended for use in python as
 * import xml.etree.ElementTree as ET
 * tree = ET.ElementTree(ET.fromstring(xmlstring))
 */
class RunConfigString
{
public:
        ///
        static std::string CreateTestsBasic1();

        ///
        static std::string CreateTestsBasic2();
};

} // namespace util
} // namespace stride
