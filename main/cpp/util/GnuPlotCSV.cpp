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
 * Source file of GnuPlotCSV.
 */

#include "GnuPlotCSV.h"

#include <boost/filesystem/fstream.hpp>

using namespace boost;
using namespace std;

namespace stride {
namespace util {

void GnuPlotCSV::WriteLabels(boost::filesystem::ofstream& file) const
{
        file << "#";
        CSV::WriteLabels(file);
}

} // namespace util
} // namespace stride
