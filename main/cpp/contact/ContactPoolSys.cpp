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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Core Population class.
 */

#include "ContactPoolSys.h"

using namespace std;
using namespace stride::ContactType;

namespace stride {

ContactPoolSys::ContactPoolSys() : m_currentContactPoolId(), m_sys()
{
        for (Id typ : IdList) {
                m_sys[typ].emplace_back(ContactPool(0U, typ));
                m_currentContactPoolId[typ] = 1;
        }
}

ContactPool* ContactPoolSys::CreateContactPool(ContactType::Id typeId)
{
        return m_sys[typeId].emplace_back(m_currentContactPoolId[typeId]++, typeId);
}

} // namespace stride
