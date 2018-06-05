#pragma once
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
 * Header file for BenchControlHelper.
 */
#include "TestDescriptors.h"

#include <regex>
#include <string>
#include <vector>

namespace myhayai {

/**
 * Assist in selecting benchmark tests based on regex's and shuffle
 * the order of execution.
 */
class BenchControlHelper
{
public:
        /// Defaults.
        BenchControlHelper(const TestDescriptors&          td,
                           const std::vector<std::string>& positive = std::vector<std::string>(),
                           const std::vector<std::string>& negative = std::vector<std::string>());

        /// Return the canonical names obtained after applying
        /// the positive and negative regex filters.
        std::vector<std::string> GetIncludedNames() const;

        /// Checks whether a name gets to be included after applying
        /// the positive and exclude negative filters.
        bool IsIncluded(const std::string& name) const;

        /// Shuffle test names.
        /// \param names  vector to be shuffled.
        void Shuffle(std::vector<std::string>& names) const;

private:
        const TestDescriptors&  m_descriptors; ///< The test descriptors to select from.
        std::vector<std::regex> m_negative;    ///< Negative regexes for excluding tests.
        std::vector<std::regex> m_positive;    ///< Positive regexes for including tests.
};

} // namespace myhayai
