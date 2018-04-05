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
 *  Author bistromatics/Robin Jadoul.
 */

/**
 * @file
 * Interface/Implementation of Any.
 */

#include <functional>
#include <map>
#include <memory>

namespace stride {
namespace util {

/**
 * A RAII void*, for type erasure, that can point to anything.
 * Any owns the pointer, and cleans up with a custom destructor.
 * Does type verification when NDEBUG is not set.
 */
class Any
{
public:
        ///
        Any()
            : m_ptr(nullptr), m_destroy([]() {})
#if not defined(NDEBUG)
              ,
              m_id("")
#endif
        {
        }

        ///
        ~Any() { m_destroy(); }

        // We don't need these right now, so this is less work.
        Any(const Any&) = delete;
        Any& operator=(const Any&) = delete;
        Any(Any&&)                 = delete;
        Any& operator=(Any&&) = delete;

        ///
        template <typename T, class... Args>
        void emplace(Args&&... args)
        {
                m_destroy();
                m_ptr     = new T(std::forward<Args>(args)...);
                m_destroy = [this]() { delete static_cast<T*>(m_ptr); };
#if not defined(NDEBUG)
                m_id = typeid(T).name();
#endif
        }

        ///
        template <typename T>
        T* cast() const
        {
#if not defined(NDEBUG)
                if (!*this) {
                        throw std::bad_cast();
                }
                if (typeid(T).name() != m_id) {
                        throw std::bad_cast();
                }
#endif
                return static_cast<T*>(m_ptr);
        }

        ///
        explicit operator bool() const { return m_ptr != nullptr; }

private:
        void*                 m_ptr;
        std::function<void()> m_destroy;
#if not defined(NDEBUG)
        std::string m_id;
#endif
};

} // namespace util
} // namespace stride
