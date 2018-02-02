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
 *  Copyright 2017, Draulans S, Van Leeuwen L
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */

#include "SocietyCSVWriter.h"

namespace stride {
namespace generator {

using namespace std;
using boost::filesystem::path;
using util::CSV;

CSV SocietyCSVWriter::WritePersons(const path& out)
{
        CSV csv{"age", "household_id", "school_id", "work_id", "primary_community", "secondary_community"};

        vector<vector<string>> rows;
        for (const auto& p : m_society.GetPersons()) {
                size_t school_id = (p.GetSchoolID() != 0) ? p.GetSchoolID() : p.GetUniversityID();
                rows.emplace_back(vector<string>{to_string(p.GetAge()), to_string(p.GetHouseholdID()),
                                                 to_string(school_id), to_string(p.GetWorkID()),
                                                 to_string(p.GetPrimaryCommunityID()),
                                                 to_string(p.GetSecondaryCommunityID())});
        }

        csv.addRows(rows);
        csv.write(out);
        return csv;
}

CSV SocietyCSVWriter::WriteCities(const path& out)
{
        CSV csv{"city_id", "city_name", "province", "population", "x_coord", "y_coord", "latitude", "longitude"};

        vector<vector<string>> rows;
        for (const auto& c : m_society.GetCities()) {
                string dummy_x = "NA", dummy_y = "NA";
                if (c.GetDummyLocation().m_point_type != PointType::Null) {
                        dummy_x = to_string(c.GetDummyLocation().m_x);
                        dummy_y = to_string(c.GetDummyLocation().m_y);
                }
                if (m_use_xy) {
                        rows.emplace_back(vector<string>{to_string(c.GetID()), c.GetName(), to_string(c.GetProvince()),
                                                         to_string(c.GetPopulation()), to_string(c.GetLocation().m_x),
                                                         to_string(c.GetLocation().m_y), dummy_y, dummy_x});
                } else {
                        rows.emplace_back(vector<string>{to_string(c.GetID()), "\"" + c.GetName() + "\"",
                                                         to_string(c.GetProvince()), to_string(c.GetPopulation()),
                                                         dummy_x, dummy_y, to_string(c.GetLocation().m_y),
                                                         to_string(c.GetLocation().m_x)});
                }
        }

        csv.addRows(rows);
        csv.write(out);
        return csv;
}

CSV SocietyCSVWriter::WriteCommunities(const path& out)
{
        CSV csv{"community_id", "community_type", "max_size", "city_id"};

        vector<vector<string>> rows;
        for (Community c : m_society.GetCommunities()) {
                string type;
                if (c.GetCommunityType() == CommunityType::School)
                        type = "school";
                else if (c.GetCommunityType() == CommunityType::University)
                        type = "university";
                else if (c.GetCommunityType() == CommunityType::Work)
                        type = "work";
                else if (c.GetCommunityType() == CommunityType::Primary)
                        type = "primary";
                else if (c.GetCommunityType() == CommunityType::Secondary)
                        type = "secondary";
                rows.emplace_back(
                    vector<string>{to_string(c.GetID()), type, to_string(c.GetSize()), to_string(c.GetCityID())});
        }

        csv.addRows(rows);
        csv.write(out);
        return csv;
}

CSV SocietyCSVWriter::WriteHouseholds(const path& out)
{
        CSV csv{"household_id", "size", "city_id"};

        vector<vector<string>> rows;
        for (const auto& h : m_society.GetHouseholds()) {
                rows.emplace_back(
                    vector<string>{to_string(h.GetID()), to_string(h.GetSize()), to_string(h.GetCityID())});
        }

        csv.addRows(rows);
        csv.write(out);
        return csv;
}

} // namespace generator
} // namespace stride
