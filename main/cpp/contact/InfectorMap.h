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
 * Header for the InfectorMap.
 */

#pragma once

#include "contact/ContactLogMode.h"
#include "contact/Infector.h"
#include "contact/InfectorExec.h"

#include <map>
#include <tuple>
#include <utility>

namespace stride {

class Calendar;
class Population;

/**
 * Mechanism to select the appropriate Infector template to execute.
 */
class InfectorMap : public std::map<std::tuple<stride::ContactLogMode::Id, bool>, InfectorExec*>
{
public:
        /// Fully initialized.
        InfectorMap()
        {
                Add<true>();
                Add<false>();
        }

private:
        /// Filling up the InfectorMap.
        template <bool B>
        void Add()
        {
                using namespace ContactLogMode;

                this->emplace(
                    std::make_pair(std::make_tuple(Id::Susceptibles, B), &Infector<Id::Susceptibles, B>::Exec));
                this->emplace(
                    std::make_pair(std::make_tuple(Id::Transmissions, B), &Infector<Id::Transmissions, B>::Exec));
                this->emplace(std::make_pair(std::make_tuple(Id::All, B), &Infector<Id::All, B>::Exec));
                this->emplace(std::make_pair(std::make_tuple(Id::None, B), &Infector<Id::None, B>::Exec));
        }
};

} // namespace stride
