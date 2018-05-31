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
 * Header file for GnuPlot container.
 */

#include "CSV.h"

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/path.hpp>

namespace stride {
namespace util {

/**
 * CSV container fit for GnuPlot data.
 * The difference with the base CSV is that a header row with labels
 * is not required (in which case it defaults to column numbering) and
 * if present it starts with a '#'.
 */
class GnuPlotCSV : public CSV
{
public:
        using CSV::CSV;

        friend boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const GnuPlotCSV& csv);

private:
        /// Write header with labels.
        void WriteLabels(boost::filesystem::ofstream& file) const override;
};

inline boost::filesystem::ofstream& operator<<(boost::filesystem::ofstream& ofs, const GnuPlotCSV& csv)
{
        csv.WriteLabels(ofs);
        csv.WriteRows(ofs);
        return ofs;
}

} // namespace util
} // namespace stride
