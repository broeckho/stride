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
 *  Copyright 2019, Jan Broeckhove and Bistromatics group.
 */

#include "GeoGridConfigBuilder.h"

#include "gengeopop/GeoGridConfig.h"
#include "gengeopop/io/ReaderFactory.h"

namespace gengeopop {

using namespace std;

void GeoGridConfigBuilder::SetData(GeoGridConfig& geoGridConfig, const string& householdsFileName)
{
        ReaderFactory readerFactory;

        auto householdsReader = readerFactory.CreateHouseholdReader(householdsFileName);
        householdsReader->SetReferenceHouseholds(geoGridConfig.popInfo.reference_households,
                                                 geoGridConfig.popInfo.persons, geoGridConfig.popInfo.contact_pools);

        const auto popSize       = geoGridConfig.input.pop_size;
        const auto averageHhSize = static_cast<double>(geoGridConfig.popInfo.persons.size()) /
                                   geoGridConfig.popInfo.reference_households.size();

        geoGridConfig.popInfo.compulsory_pupils =
            static_cast<unsigned int>(floor(householdsReader->GetFractionCompulsoryPupils() * popSize));

        geoGridConfig.popInfo.popcount_1865 =
            static_cast<unsigned int>(floor(householdsReader->GetFraction1865Years() * popSize));

        geoGridConfig.popInfo.popcount_1826 =
            static_cast<unsigned int>(floor(householdsReader->GetFraction1826Years() * popSize));

        geoGridConfig.popInfo.popcount_1826_student = static_cast<unsigned int>(
            floor(geoGridConfig.input.fraction_1826_student * geoGridConfig.popInfo.popcount_1826));

        geoGridConfig.popInfo.popcount_1865_active = static_cast<unsigned int>(
            floor(geoGridConfig.input.fraction_1865_active *
                  (geoGridConfig.popInfo.popcount_1865 - geoGridConfig.popInfo.popcount_1826_student)));

        geoGridConfig.popInfo.households =
            static_cast<unsigned int>(floor(static_cast<double>(popSize) / averageHhSize));
}

} // namespace gengeopop
