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

#include "GeoGridConfig.h"

#include "gengeopop/io/CitiesReader.h"
#include "io/HouseholdReader.h"
#include "util/StringUtils.h"

namespace gengeopop {

using stride::util::intToDottedString;
using namespace std;
using namespace boost::property_tree;

GeoGridConfig::GeoGridConfig() : input{}, calculated{}, generated{}, constants{} {}

GeoGridConfig::GeoGridConfig(const ptree& configPt)
        : GeoGridConfig()
{
        input.populationSize = configPt.get<unsigned int>("run.geopop_gen.population_size");
        input.fraction_1826_years_WhichAreStudents
                = configPt.get<double>("run.geopop_gen.fraction_1826_years_which_are_students");
        input.fraction_active_commutingPeople = configPt.get<double>("run.geopop_gen.fraction_active_commuting_people");
        input.fraction_student_commutingPeople = configPt.get<double>("run.geopop_gen.fraction_student_commuting_people");
        input.fraction_1865_years_active = configPt.get<double>("run.geopop_gen.fraction_1865_years_active");
}

void GeoGridConfig::Calculate(shared_ptr<GeoGrid> geoGrid, shared_ptr<HouseholdReader> householdReader)
{
        calculated.compulsoryPupils = static_cast<unsigned int>(
            floor(householdReader->GetFractionCompulsoryPupils() * input.populationSize));
        calculated.popcount_1865_years =
            static_cast<unsigned int>(floor(householdReader->GetFraction1865Years() * input.populationSize));
        calculated.popcount_1826_years =
            static_cast<unsigned int>(floor(householdReader->GetFraction1826Years() * input.populationSize));
        calculated.popcount_1826_years_and_student = static_cast<unsigned int>(
            floor(input.fraction_1826_years_WhichAreStudents * calculated.popcount_1826_years));

        calculated.popcount_1865_and_years_active = static_cast<unsigned int>(
            floor(input.fraction_1865_years_active *
                       (calculated.popcount_1865_years - calculated.popcount_1826_years_and_student)));

        calculated.households = static_cast<unsigned int>(
            floor(static_cast<double>(input.populationSize) / householdReader->AverageHouseholdSize()));

        generated.reference_households = std::move(householdReader->GetHouseHolds());

        for (const shared_ptr<Location>& loc : *geoGrid) {
                loc->CalculatePopulation(input.populationSize);
        }
}

ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int width = 53;
        out << left << "Input:" << endl;
        out << left << setw(width) << "Fraction of active commuting"
            << config.input.fraction_active_commutingPeople << endl;
        out << left << setw(width) << "Fraction of students commuting"
            << config.input.fraction_student_commutingPeople << endl;
        out << left << setw(width) << "Fraction 18-65 (without students) which are active"
            << config.input.fraction_1865_years_active << endl;
        out << left << setw(width) << "Fraction 18-26 years which are students"
            << config.input.fraction_1826_years_WhichAreStudents << endl;
        out << left << setw(width) << "Population size" << intToDottedString(config.input.populationSize)
            << endl;
        out << endl;
        out << left << "Calculated:" << endl;
        out << left << setw(width) << "Compulsory pupils"
            << intToDottedString(config.calculated.compulsoryPupils) << endl;
        out << left << setw(width) << "18-26 years"
            << intToDottedString(config.calculated.popcount_1826_years) << endl;
        out << left << setw(width) << "18-26 years which are student"
            << intToDottedString(config.calculated.popcount_1826_years_and_student) << endl;
        out << left << setw(width) << "18-65 years"
            << intToDottedString(config.calculated.popcount_1865_years) << endl;
        out << left << setw(width) << "18-65 years which are active"
            << intToDottedString(config.calculated.popcount_1865_and_years_active) << endl;
        out << endl;
        return out;
}

} // namespace gengeopop
