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
 * Interface/Implementation of Subject.
 */

#include <algorithm>
#include <memory>

namespace stride {
namespace python {

/// Template for Subject/Observer. Offers flexibility in defining event types.
template <typename E, typename U>
class Subject
{
public:
        virtual ~Subject() { UnregisterAll(); }

        void Register(const std::shared_ptr<U>&);

        void Unregister(const std::shared_ptr<U>&);

        void UnregisterAll();

        void Notify(const E&);

private:
        std::vector<std::shared_ptr<U>> m_observers;
};

template <typename E, typename U>
void Subject<E, U>::Register(const std::shared_ptr<U>& u)
{
        m_observers.push_back(u);
}

template <typename E, typename U>
void Subject<E, U>::Unregister(const std::shared_ptr<U>& u)
{
        m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), u), m_observers.end());
}

template <typename E, typename U>
void Subject<E, U>::UnregisterAll()
{
        m_observers.clear();
}

template <typename E, typename U>
void Subject<E, U>::Notify(const E& e)
{
        for (const auto& o : m_observers) {
                // const auto spt = o.lock();
                // if (spt) {
                // spt->Update(e);
                //} else {
                //	m_observers.erase(std::remove(m_observers.begin(), m_observers.end(),
                // o), m_observers.end());
                //}
                o->Update(e);
        }
}

} // namespace python
} // namespace stride
