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

/**
 * @file
 * Implementation file for LocalDiscussion.
 */

#include "LocalDiscussion.h"

namespace stride {

double LocalDiscussion::m_discussion_chance = 0.01;

void LocalDiscussion::Update(Person* p1, Person* p2)
{
	p1->GetBelief()->Update(p2->GetBelief());
	p2->GetBelief()->Update(p1->GetBelief());
}

double LocalDiscussion::GetDiscussionChance()
{
	return m_discussion_chance;
}

} // end-of-namespace
