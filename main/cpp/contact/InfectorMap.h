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
 * Header for the InfectorMap.
 */

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
 * Simulator can time step and reveal some of the key data.
 * The Subject base class used for the interaction with the python environment only.
 */
class InfectorMap : public std::map<std::tuple<stride::ContactLogMode::Id, bool, std::string>, InfectorExec*>
{
public:
        /// Fully initialized.
        InfectorMap()
        {
                Add<NoLocalInformation, true>("NoLocalInformation");
                Add<NoLocalInformation, false>("NoLocalInformation");
                Add<LocalDiscussion, true>("LocalDiscussion");
                Add<LocalDiscussion, false>("LocalDiscussion");
        }

private:
        /// Filling up the InfectorMap.
        template <typename T, bool B>
        void Add(const std::string& name)
        {
                using namespace ContactLogMode;
                const auto Suscep = Id::Susceptibles;
                const auto Transm = Id::Transmissions;

                this->emplace(make_pair(make_tuple(Suscep, B, name), &Infector<Suscep, B, T>::Exec));
                this->emplace(make_pair(make_tuple(Transm, B, name), &Infector<Transm, B, T>::Exec));
                this->emplace(make_pair(make_tuple(Id::All, B, name), &Infector<Id::All, B, T>::Exec));
                this->emplace(make_pair(make_tuple(Id::None, B, name), &Infector<Id::None, B, T>::Exec));
        }
};

} // namespace stride
