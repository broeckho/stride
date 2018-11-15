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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#pragma once
#include <map>
#include <string>
#include <util/LogUtils.h>
#include <vector>

#include "BoxplotData.h"

namespace calibration {

/**
 * A class used to generate QtCharts Boxplots
 */
class BoxplotGenerator
{
public:
        /// Setup
        BoxplotGenerator();

        /// Display the boxplots
        void Display(std::vector<BoxplotData> data) const;

        /// Write the boxplots to files
        void WriteToFile(std::vector<BoxplotData> data) const;

private:
        void GeneratePlots(std::vector<BoxplotData> data,
                           bool write) const;   ///< Generate the actual plots. Write them to files if write is true,
                                                ///< otherwise display them on screen.
        std::shared_ptr<spdlog::logger> logger; ///< Logger to use for this class
};

} // namespace calibration
