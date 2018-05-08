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
 *  Copyright 2017, 2018, Kuylen E, Willem L, Broeckhove J
 */

/**
 * @file
 * Header file for Belief.
 */

namespace stride {

/**
 * Base class for all belief policies.
 */
class Belief
{
public:
        ///
        Belief() {}

        ///
        virtual ~Belief() {}

        ///
        // boost::property_tree::ptree Get() { return m_pt; }

        ///
        // void Set(const boost::property_tree::ptree& pt) { m_pt = pt; }

        virtual bool HasAdopted() const { return false; }

private:

};

} // namespace stride
