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
 *  Copyright 2017, Kuylen E, Willem L, Broeckhove J
 */

#include "pop/Person.h"
#include "util/RNManager.h"

namespace stride {

/**
 * Class for local information policy 'LocalDiscussion'.
 * Upon contact, members of a contactpool discuss their beliefs/health status/...
 * with each other with a certain probability.
 * TODO make this probability configurable
 */
class LocalDiscussion
{
public:
        static void Update(Person*, Person*)
        {
                // TODO discussion probability
                // TODO if discussion: update p1
                // TODO if discussion: update p2
        }

private:
        static double m_discussion_probability;
};

} // namespace stride
