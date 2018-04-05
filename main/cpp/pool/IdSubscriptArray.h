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

#include <array>
#include <initializer_list>
#include <iostream>
#include <stdexcept>

namespace stride {
namespace ContactPoolType {

/**
 * An std::array modified to enable subscripting with the constact pool
 * type indentifiers. The bounds checking "at" method, to protevt against
 * a subscript that is static_cast<Id> from an erroneous integer value.
 *
 * We intentionally shadow the direct subscripting of the base array.
 *
 * Writing constructor is somehat weird since std::array has no constructors,
 * only aggregate initialization. But we want some constructors for
 * shorthand expressions.
 *
 * @tparam T  base type of the array.
 */
template <class T>
class IdSubscriptArray : public std::array<T, NumOfTypes()>
{
public:
        /// What we 'll use most often and where we can have a default and
        /// initialize all array elements to the same value.
        /// e.g.    IdSubscriptArray<bool> m(true);
        explicit IdSubscriptArray(T t = T())
        {
                for (auto typ : IdList) {
                        this->operator[](typ) = t;
                }
        }

        /// When we want to use an initializer list the elements is the
        /// (possibly emty) initializer list are applied to the first
        /// elements in order; any remaininig elements are default initialized.
        ///  e.g.   IdSubscriptArray<bool> mm {true, false, true};
        IdSubscriptArray(std::initializer_list<T> l)
        {
                auto it = l.begin();
                for (auto typ : IdList) {
                        if (it != l.end()) {
                                this->operator[](typ) = *it;
                                ++it;
                        } else {
                                this->operator[](typ) = T();
                        }
                }
        }

        /// This actually works in itself but interferes annoyingly with the first
        /// constructor above and is for practical purpose redundant.
        ///
        /// Forward constructor arguments to base class.
        /// template <typename... Args>
        /// explicit IdSubscriptArray(Args&&... args) : std::array<T, NumOfTypes()>{{std::forward<Args>(args)...}}{}

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::reference& operator[](ContactPoolType::Id id)
        {
                // std::cerr << "haha"<< std::endl;
                return this->std::template array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::const_reference& operator[](ContactPoolType::Id id) const
        {
                return this->std::template array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::reference& at(ContactPoolType::Id id)
        {
                if (ToSizeT(id) >= NumOfTypes()) {
                        throw std::out_of_range("IdSubscriptArray::at> Id out of range");
                }
                return this->std::template array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }

        /// Subscripting with pool typ id as argument.
        typename std::array<T, NumOfTypes()>::const_reference& at(ContactPoolType::Id id) const
        {
                if (ToSizeT(id) >= NumOfTypes()) {
                        throw std::out_of_range("IdSubscriptArray::at> Id out of range");
                }
                return this->std::template array<T, NumOfTypes()>::operator[](ContactPoolType::ToSizeT(id));
        }
};

} // namespace ContactPoolType
} // namespace stride
