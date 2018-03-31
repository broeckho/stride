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
 * Container for the contact pools of various type (household, work, ...)..
 */

#include "pool/ContactPool.h"
#include "pool/ContactPoolType.h"

#include <stdexcept>

namespace stride {
namespace ContactPoolType {

/**
 * The std::array modified to enable subscripting with the constact pool
 * type indentifiers. The bounds checking "at" method, to protevt against
 * a subscript that is static_cast<Id> from an erroneous integer value.
 * We intentionally shadow the direct subscripting of the base array.
 *
 * @tparam T  base type of the array.
 */
template <class T>
class IdSubscriptArray : public std::array<T, NumOfTypes()>
{
public:
        /// Forwrd constructor argumenents to bas class.
        template <typename... Args>
        explicit IdSubscriptArray(Args&&... args) : std::array<T, NumOfTypes()>{std::forward<Args>(args)...}
        {
        }

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::reference& operator[](ContactPoolType::Id id)
        {
                return (*this).std::array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::const_reference& operator[](ContactPoolType::Id id) const
        {
                return (*this).std::array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::reference& at(ContactPoolType::Id id)
        {
                if (ToSizeT(id) >= NumOfTypes()) {
                        throw std::out_of_range("IdSubscriptArray::at> Id out of range");
                }
                return (*this).std::array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::const_reference& at(ContactPoolType::Id id) const
        {
                if (ToSizeT(id) >= NumOfTypes()) {
                        throw std::out_of_range("IdSubscriptArray::at> Id out of range");
                }
                return (*this).std::array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }
};

} // namespace ContactPoolType
} // namespace stride
