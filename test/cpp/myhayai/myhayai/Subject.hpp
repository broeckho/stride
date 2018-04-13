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
 * Definition of Subject/Observer.
 */

#include <functional>
#include <map>
#include <memory>
#include <utility>

namespace myhayai {

/**
 * Template for Subject/Observer (or Publish/Subscribe). Offers flexibility
 * in defining event types and having customized callbacks. Despite the
 * shared_ptrs in the Register/Unregister, the Subject takes no ownership
 * of the observer object and only stores a weak_ptr.
 */
template <typename PayloadType>
class Subject
{
public:
        using CallbackType = std::function<void(const PayloadType&)>;

public:
        Subject() = default;

        virtual ~Subject() { UnregisterAll(); }

        template <typename U>
        void Register(const std::shared_ptr<U>& u, CallbackType f)
        {
                m_observers.insert(std::make_pair(std::static_pointer_cast<const void>(u), f));
        }

        template <typename U>
        void Unregister(const std::shared_ptr<U>& u)
        {
                m_observers.erase(std::static_pointer_cast<const void>(u));
        }

        void UnregisterAll() { m_observers.clear(); }

        void Notify(const PayloadType& e)
        {
                for (const auto& o : m_observers) {
                        const auto spt = o.first.lock();
                        if (spt) {
                                (o.second)(e);
                        } else {
                                m_observers.erase(o.first);
                        }
                }
        }

private:
        std::map<std::weak_ptr<const void>, CallbackType, std::owner_less<std::weak_ptr<const void>>> m_observers;
};

} // namespace myhayai
