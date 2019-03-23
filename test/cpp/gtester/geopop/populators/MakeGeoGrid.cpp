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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "MakeGeoGrid.h"

#include "geopop/Location.h"
#include "geopop/generators/HouseholdGenerator.h"
#include "geopop/generators/K12SchoolGenerator.h"
#include "pop/Population.h"
#include "util/RnMan.h"

using namespace std;
using namespace stride;
using namespace stride::util;
using namespace stride::ContactType;
using namespace geopop;

/**
 * Builds a GeoGrid in the population with all Locations at Coordinate (0.0, 0.0), all having
 * the same population count, the same number of K12 schools, the same number of households,
 * each having the same number of persons.
 * @param locCount          The number of Locations.
 * @param locPop            The population count at each Location.
 * @param schoolCount       The number of K12Schools at each Location.
 * @param houseHoldCount    The number of households at each Location.
 * @param personCount       The number of persons per Household.
 * @param pop               The population carrying this GeoGrid.
 */
void MakeGeoGrid(const GeoGridConfig& ggConfig, int locCount, int locPop, int schoolCount, int houseHoldCount,
                 int personCount, Population* pop)
{
        vector<unsigned int> populationSample = {
            17, 27, 65, 40, 29, 76, 27, 50, 28, 62, 50, 14, 30, 36, 12, 31, 25, 72, 62, 4,  40, 52, 55, 50, 62,
            1,  30, 23, 6,  71, 50, 65, 8,  26, 44, 76, 23, 22, 69, 22, 4,  22, 48, 12, 57, 42, 36, 45, 73, 13,
            18, 58, 37, 43, 70, 36, 11, 54, 26, 8,  7,  6,  76, 15, 5,  23, 34, 74, 17, 33, 23, 54, 43, 72, 46,
            9,  57, 53, 75, 45, 29, 58, 67, 36, 63, 29, 24, 7,  35, 53, 40, 56, 60, 62, 53, 68, 78, 73, 24, 49,
            62, 12, 17, 58, 19, 77, 34, 40, 11, 65, 75, 27, 3,  34, 75, 1,  3,  28, 6,  72, 50, 69, 40, 42, 36,
            33, 77, 71, 47, 60, 53, 22, 13, 43, 62, 35, 47, 3,  68, 15, 53, 75, 2,  62, 54, 1,  32, 71, 55, 58,
            42, 69, 29, 15, 64, 66, 43, 15, 5,  70, 41, 43, 18, 19, 16, 57, 62, 12, 75, 76, 6,  67, 61, 11, 15,
            22, 51, 67, 38, 74, 66, 70, 1,  16, 71, 15, 40, 38, 11, 48, 45, 59, 11, 32, 18, 40, 46, 65, 21, 59,
            22, 2,  75, 5,  15, 10, 21, 59, 11, 61, 11, 9,  76, 76, 4,  66, 74, 74, 20, 18, 38, 5,  53, 29, 76,
            21, 73, 54, 43, 45, 12, 71, 70, 72, 36, 28, 18, 77, 53, 37, 40, 51, 71, 45, 18, 51, 22, 9,  17, 48,
            76, 73, 9,  27, 5,  68, 25, 16, 29, 58, 78, 75, 40, 8,  37, 63, 63, 76, 55, 47, 18, 4,  21, 39, 45,
            42, 20, 41, 40, 37, 38, 30, 48, 9,  40, 23, 68, 77, 21, 50, 18, 27, 54, 1,  32, 67, 27, 14, 4,  78};

        const auto         populationSize{populationSample.size()};
        GeoGridConfig      config{};
        auto&              geoGrid = pop->RefGeoGrid();
        RnMan              rnMan(RnInfo{});
        K12SchoolGenerator k12Gen(rnMan);
        HouseholdGenerator hhGen(rnMan);
        const unsigned int pph   = ggConfig.pools.pools_per_household;
        const unsigned int ppk12 = ggConfig.pools.pools_per_k12school;

        size_t sampleId = 0;
        auto   personId = 0U;
        for (int locI = 0; locI < locCount; locI++) {
                auto loc = make_shared<Location>(locI, 1, Coordinate(0.0, 0.0), "", locPop);

                for (int schI = 0; schI < schoolCount; schI++) {
                        k12Gen.AddPools(*loc, pop, ppk12);
                }

                for (int hI = 0; hI < houseHoldCount; hI++) {
                        hhGen.AddPools(*loc, pop, pph);
                        auto contactPool = loc->RefPools(Id::Household).back();

                        for (int i = 0; i < personCount; i++) {
                                auto sample = populationSample[sampleId % populationSize];
                                auto p      = pop->CreatePerson(personId, sample, contactPool->GetId(), 0, 0, 0, 0, 0);
                                contactPool->AddMember(p);
                                sampleId++;
                                personId++;
                        }
                }
                geoGrid.AddLocation(loc);
        }
}
