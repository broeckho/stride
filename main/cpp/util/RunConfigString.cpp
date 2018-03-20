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
 * Implementation of ptree utils.
 */

#include "RunConfigString.h"
#include "RunConfigPtree.h"

#include <boost/property_tree/xml_parser.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace boost::property_tree;

namespace stride {
namespace util {

string RunConfigString::CreateTestsBasic1()
{
        ostringstream ss;
        write_xml(ss, RunConfigPtree::CreateTestsBasic1(), xml_writer_make_settings<ptree::key_type>(' ', 8));
        cout << ss.str() << endl;
        return ss.str();
}

string RunConfigString::CreateTestsBasic2()
{
        ostringstream ss;
        write_xml(ss, RunConfigPtree::CreateTestsBasic2(), xml_writer_make_settings<ptree::key_type>(' ', 8));
        cout << ss.str() << endl;
        return ss.str();
}

} // namespace util
} // namespace stride
