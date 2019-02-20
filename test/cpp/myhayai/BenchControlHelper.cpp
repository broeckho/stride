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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */
/**
 * @file
 * Implementation file for BenchControlHelper.
 */

#include "BenchControlHelper.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <locale>
#include <random>
#include <regex>
#include <stdexcept>

using namespace std;

namespace myhayai {

BenchControlHelper::BenchControlHelper(const TestDescriptors& td, const std::vector<std::string>& positive,
                                       const std::vector<std::string>& negative)
    : m_descriptors(td), m_negative(), m_positive()
{
        for (const auto& r : positive) {
                m_positive.emplace_back(regex(r));
        }
        for (const auto& r : negative) {
                m_negative.emplace_back(regex(r));
        }
}
vector<string> BenchControlHelper::GetIncludedNames() const
{
        vector<string> includedNames;
        for (const auto& item : m_descriptors) {
                const auto& t_n = item.first;
                if (IsIncluded(t_n)) {
                        includedNames.emplace_back(t_n);
                }
        }
        return includedNames;
}

bool BenchControlHelper::IsIncluded(const std::string& name) const
{
        bool include = m_positive.empty();
        for (const auto& re : m_positive) {
                if (regex_search(name, re)) {
                        include = true;
                        break;
                }
        }
        if (include) {
                for (const auto& re : m_negative) {
                        if (regex_search(name, re)) {
                                include = false;
                                break;
                        }
                }
        }
        return include;
}

void BenchControlHelper::Shuffle(vector<string>& names) const
{
        random_device rd;
        mt19937       g(rd());
        shuffle(names.begin(), names.end(), g);
}

} // namespace myhayai
