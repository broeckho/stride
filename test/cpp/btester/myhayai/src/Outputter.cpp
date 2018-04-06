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
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory one level higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for Outputter.
 */

#include "myhayai/Outputter.hpp"

#include "myhayai/TestDescriptor.hpp"
#include "myhayai/TestResult.hpp"

#include <cstddef>
#include <iostream>

using namespace std;

namespace myhayai {

void Outputter::WriteTestNameToStream(ostream& stream, const string& fixtureName, const string& testName,
                                      const TestParametersDescriptor& parameters)
{
        stream << fixtureName << "." << testName;
        const vector<TestParameterDescriptor>& descs = parameters.Parameters();
        if (!descs.empty()) {
                stream << "(";
                for (size_t i = 0; i < descs.size(); ++i) {
                        if (i) {
                                stream << ", ";
                        }
                        const TestParameterDescriptor& desc = descs[i];
                        stream << desc.Declaration << " = " << desc.Value;
                }
                stream << ")";
        }
}

} // namespace myhayai
