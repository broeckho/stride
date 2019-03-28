
#include "HouseholdJSONReader.h"
#include "util/Exception.h"

using json = nlohmann::json;

namespace geopop {

    using namespace std;
    using namespace stride::util;

    HouseholdJSONReader::HouseholdJSONReader(std::unique_ptr<std::istream> inputStream) : m_input_stream(std::move(inputStream))
    {
    }

    void HouseholdJSONReader::SetReferenceHouseholds(unsigned int &ref_person_count,
                                                     std::vector<std::vector<unsigned int>> &ref_ages)
    {
        json json_file;

        try {
            json_file = json::parse(*(m_input_stream.get()));
        } catch (json::parse_error&) {
            throw Exception("Problem parsing JSON file, check whether empty or invalid JSON.");
        }

        unsigned int p_count = 0U;
        for(const json array : json_file["householdsList"]) {
            vector<unsigned int> temp = array;
            p_count += temp.size();
            ref_ages.emplace_back(temp);
        }
        ref_person_count = p_count;
    }

} // namespace geopop