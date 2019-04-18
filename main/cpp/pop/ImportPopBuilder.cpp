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
 *  Copyright 2018, 2019 Jan Broeckhove and Bistromatics group.
 */

/**
 * @file
 * Initialize populations: implementation.
 */

#include "ImportPopBuilder.h"

#include "geopop/GeoGrid.h"
#include "geopop/io/GeoGridReader.h"
#include "geopop/io/GeoGridReaderFactory.h"
#include "pop/Population.h"
#include "util/LogUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <spdlog/logger.h>

namespace stride {

using namespace std;
using namespace boost::property_tree;
using namespace geopop;

shared_ptr<Population> ImportPopBuilder::Build(shared_ptr<Population> pop)
{
        const auto importFile = m_config.get<string>("run.population_file");
        m_stride_logger->info("Importing population from file {}.", importFile);

        GeoGridReaderFactory geoGridReaderFactory;
        const auto&          reader = geoGridReaderFactory.CreateReader(importFile, pop.get());
        reader->Read();
        pop->RefGeoGrid().Finalize();

        return pop;
}

} // namespace stride
