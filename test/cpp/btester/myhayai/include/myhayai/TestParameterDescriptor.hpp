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
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */

/**
 * @file
 * Header file for TestParameterDescriptor.
 */

#include "myhayai/Fixture.hpp"
#include "myhayai/TestFactory.hpp"

#include <string>
#include <utility>

namespace myhayai {

/// Parameter declaration. Describes parameter type and name.
struct TestParameterDescriptor
{
        TestParameterDescriptor(std::string declaration, std::string value)
            : Declaration(std::move(declaration)), Value(std::move(value))
        {
        }

        std::string Declaration;
        std::string Value;
};

} // namespace myhayai
