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

#include "gengeopop/Location.h"
#include "gengeopop/io/CitiesReader.h"
#include "gengeopop/io/HouseholdReader.h"
#include "util/StringUtils.h"

namespace gengeopop {

using stride::util::intToDottedString;
using namespace std;
using namespace boost::property_tree;

GeoGridConfig::GeoGridConfig() : input{}, calculated{}, generated{}, constants{} {}

GeoGridConfig::GeoGridConfig(const ptree& configPt)
        : GeoGridConfig()
{
        input.pop_size = configPt.get<unsigned int>("run.geopop_gen.population_size");
        input.fraction_1826_student = configPt.get<double>("run.geopop_gen.fraction_1826_years_which_are_students");
        input.fraction_active_commuters = configPt.get<double>("run.geopop_gen.fraction_active_commuting_people");
        input.fraction_student_commuters = configPt.get<double>("run.geopop_gen.fraction_student_commuting_people");
        input.fraction_1865_active = configPt.get<double>("run.geopop_gen.fraction_1865_years_active");
}

void GeoGridConfig::Calculate(shared_ptr<GeoGrid> geoGrid, shared_ptr<HouseholdReader> householdReader)
{
        calculated.compulsory_pupils = static_cast<unsigned int>(
            floor(householdReader->GetFractionCompulsoryPupils() * input.pop_size));

        calculated.popcount_1865 =
            static_cast<unsigned int>(floor(householdReader->GetFraction1865Years() * input.pop_size));

        calculated.popcount_1826 =
            static_cast<unsigned int>(floor(householdReader->GetFraction1826Years() * input.pop_size));

        calculated.popcount_1826_student = static_cast<unsigned int>(
            floor(input.fraction_1826_student * calculated.popcount_1826));

        calculated.popcount_1865_active = static_cast<unsigned int>(
            floor(input.fraction_1865_active *
                       (calculated.popcount_1865 - calculated.popcount_1826_student)));

        auto averageHhSize = static_cast<double>(householdReader->GetTotalPersonsInHouseholds())
                / generated.reference_households.size();
        calculated.households = static_cast<unsigned int>(floor(static_cast<double>(input.pop_size) / averageHhSize));

        for (const shared_ptr<Location>& loc : *geoGrid) {
                loc->SetPopCount(input.pop_size);
        }
}

ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int width = 53;
        out << left << "Input:" << endl;
        out << left << setw(width) << "Fraction of active commuting"
            << config.input.fraction_active_commuters << endl;
        out << left << setw(width) << "Fraction of students commuting"
            << config.input.fraction_student_commuters << endl;
        out << left << setw(width) << "Fraction 18-65 (without students) which are active"
            << config.input.fraction_1865_active << endl;
        out << left << setw(width) << "Fraction 18-26 years which are students"
            << config.input.fraction_1826_student << endl;
        out << left << setw(width) << "Population size" << intToDottedString(config.input.pop_size)
            << endl;
        out << endl;
        out << left << "Calculated:" << endl;
        out << left << setw(width) << "Compulsory pupils"
            << intToDottedString(config.calculated.compulsory_pupils) << endl;
        out << left << setw(width) << "18-26 years"
            << intToDottedString(config.calculated.popcount_1826) << endl;
        out << left << setw(width) << "18-26 years which are student"
            << intToDottedString(config.calculated.popcount_1826_student) << endl;
        out << left << setw(width) << "18-65 years"
            << intToDottedString(config.calculated.popcount_1865) << endl;
        out << left << setw(width) << "18-65 years which are active"
            << intToDottedString(config.calculated.popcount_1865_active) << endl;
        out << endl;
        return out;
}

} // namespace gengeopop
