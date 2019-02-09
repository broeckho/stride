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

#include "gengeopop/io/HouseholdReader.h"
#include "util/StringUtils.h"

namespace gengeopop {

using stride::util::intToDottedString;
using namespace std;
using namespace boost::property_tree;

GeoGridConfig::GeoGridConfig() : input{}, popInfo{}, constants{} {}

GeoGridConfig::GeoGridConfig(const ptree& configPt) : GeoGridConfig()
{
        input.pop_size                  = configPt.get<unsigned int>("run.geopop_gen.population_size");
        input.fraction_1826_student     = configPt.get<double>("run.geopop_gen.fraction_1826_years_which_are_students");
        input.fraction_active_commuters = configPt.get<double>("run.geopop_gen.fraction_active_commuting_people");
        input.fraction_student_commuters = configPt.get<double>("run.geopop_gen.fraction_student_commuting_people");
        input.fraction_1865_active       = configPt.get<double>("run.geopop_gen.fraction_1865_years_active");
}

ostream& operator<<(ostream& out, const GeoGridConfig& config)
{
        const int width = 53;
        out << left << "Input:" << endl;
        out << left << setw(width) << "Fraction of active commuting" << config.input.fraction_active_commuters << endl;
        out << left << setw(width) << "Fraction of students commuting" << config.input.fraction_student_commuters
            << endl;
        out << left << setw(width) << "Fraction 18-65 (without students) which are active"
            << config.input.fraction_1865_active << endl;
        out << left << setw(width) << "Fraction 18-26 years which are students" << config.input.fraction_1826_student
            << endl;
        out << left << setw(width) << "Population size" << intToDottedString(config.input.pop_size) << endl;
        out << endl;
        out << left << "Calculated:" << endl;
        out << left << setw(width) << "Compulsory pupils" << intToDottedString(config.popInfo.compulsory_pupils)
            << endl;
        out << left << setw(width) << "18-26 years" << intToDottedString(config.popInfo.popcount_1826) << endl;
        out << left << setw(width) << "18-26 years which are student"
            << intToDottedString(config.popInfo.popcount_1826_student) << endl;
        out << left << setw(width) << "18-65 years" << intToDottedString(config.popInfo.popcount_1865) << endl;
        out << left << setw(width) << "18-65 years which are active"
            << intToDottedString(config.popInfo.popcount_1865_active) << endl;
        out << endl;
        return out;
}

} // namespace gengeopop
