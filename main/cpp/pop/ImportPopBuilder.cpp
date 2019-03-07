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
#include "util/LogUtils.h"

#include <boost/property_tree/ptree.hpp>
#include <spdlog/common.h>
#include <spdlog/fmt/ostr.h>

namespace stride {

using namespace std;
using namespace boost::property_tree;
using namespace geopop;

shared_ptr<Population> ImportPopBuilder::Build(shared_ptr<Population> pop)
{
        auto stride_logger = spdlog::get("stride_logger");

        const auto importFile = m_config_pt.get<string>("run.population_file");

        GeoGridReaderFactory             geoGridReaderFactory;
        const shared_ptr<GeoGridReader>& reader = geoGridReaderFactory.CreateReader(importFile, pop.get());

        //stride_logger->debug("Importing population from " + importFile);

        pop->m_geoGrid = reader->Read();
        pop->m_geoGrid->Finalize();

        return pop;
}

} // namespace stride
