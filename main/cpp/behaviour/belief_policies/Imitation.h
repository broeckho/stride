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

/**
 * @file
 * Header file for Imitation belief policy.
 */

#include "behaviour/belief_policies/Belief.h"

namespace stride {

class Imitation: public Belief
{
public:
	Imitation(const boost::property_tree::ptree& pt) : Belief(pt)
	{
		// TODO initialize belief variables through ptree
	}

	bool HasAdopted() const
	{
		return false;
		// TODO make dependent on state of belief variables
	}
};

} // end-of-namespace
