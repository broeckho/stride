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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Multi-block CSV file (fashioned after and used for GnuPlot data files).
 */

#include "GnuPlotCSV.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>
#include <vector>

namespace stride {
namespace util {

/**
 * A collection of GnuPlotCSV's elements. It reflects a data file structure
 * with (possibly) multiple blocks separated by blank lines. Each block has
 * a CSV format. The CSV may have a header row with labels, but if present
 * the row must start with a '#'.
 */
class GnuPlot : protected std::vector<GnuPlotCSV>
{
public:
        GnuPlot() = default;

        /// Iterators.
        using std::vector<GnuPlotCSV>::begin;
        using std::vector<GnuPlotCSV>::end;
        using std::vector<GnuPlotCSV>::size;

        /// Add a block.
        void Add(const GnuPlotCSV& gpCsv) { this->emplace_back(gpCsv); }

        /// Write CSV to file.
        void Write(const boost::filesystem::path& path) const;

        friend boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const GnuPlot& gp);
};

inline boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const GnuPlot& gp)
{
        for (const GnuPlotCSV& csv : gp) {
                ofs << csv << std::endl;
                ofs << " \n " << std::endl;
        }
        return ofs;
}

} // namespace util
} // namespace stride
