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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#include "Community.h"

#include "Household.h"
#include "pop/Person.h"
#include "util/Exception.h"

using namespace stride::ContactPoolType;

namespace gengeopop {
/*
void Community::AddHouseHold(std::shared_ptr<Household> household)
{
        if (m_pools.empty()) {
                throw stride::util::Exception("Could not add the Household to Community, no ContactPool available");
        } else {
                if (household->GetPools().empty())
                        return;
                for (stride::Person* person : **household->begin()) {
                        m_pools[0]->AddMember(person);
                }
        }
}
*/
} // namespace
