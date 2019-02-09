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

/**
 * @file
 * Initialize populations.
 */

#pragma once

#include "AbstractPopBuilder.h"

#include <boost/property_tree/ptree_fwd.hpp>

#include <memory>

namespace stride {

class Population;

/**
 * Initializes Population objects.
 */
class GenPopBuilder : public AbstractPopBuilder
{
public:
        /// Use constructor of base.
        using AbstractPopBuilder::AbstractPopBuilder;

        /// Creates a synthetic population by generating it.
        std::shared_ptr<Population> Build(std::shared_ptr<Population> pop) override;
};

} // namespace stride
