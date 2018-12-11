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

/**
 * @file
 * Initialize populations: implementation.
 */

#include "ImportPopBuilder.h"

//#include "util/FileSys.h"
#include "util/LogUtils.h"
//#include "util/RnMan.h"
//#include "util/StringUtils.h"

//#include <boost/property_tree/ptree.hpp>
//#include <gengeopop/GenGeoPopController.h>
//#include <gengeopop/GeoGridConfig.h>
#include "gengeopop/io/GeoGridReaderFactory.h"

#include <spdlog/common.h>
#include <spdlog/fmt/ostr.h>

namespace stride {

using namespace std;
using namespace boost::property_tree;
using namespace gengeopop;

shared_ptr<Population> ImportPopBuilder::Build(shared_ptr<Population> pop)
{
        auto stride_logger = spdlog::get("stride_logger");

        const auto importFile = m_config_pt.get<string>("run.geopop_import_file");

        GeoGridReaderFactory                  geoGridReaderFactory;
        const shared_ptr<GeoGridReader>& reader = geoGridReaderFactory.CreateReader(importFile, pop.get());

        stride_logger->debug("Importing population from " + importFile);

        pop->m_geoGrid = reader->Read();
        pop->m_geoGrid->Finalize();

        return pop;
}

} // namespace
