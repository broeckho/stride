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

namespace stride {

/**
 * Initializes Population objects.
 */
class ImportPopBuilder : public AbstractPopBuilder
{
public:
        /// Use constructor of base.
        using AbstractPopBuilder::AbstractPopBuilder;

        /// Creates a population by importing it
        std::shared_ptr<Population> Build(std::shared_ptr<Population> pop) override;
};

} // namespace
