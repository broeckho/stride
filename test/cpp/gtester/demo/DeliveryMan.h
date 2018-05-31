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
 * Header file for DeliveryMan.
 */

#include <iostream>

/// Delivery man.
class DeliveryMan
{
public:
        /// Initialize a delivery man instance.
        /// @param speed Delivery man speed from 1 to 10.
        explicit DeliveryMan(unsigned int speed = 1) : m_speed(speed) {}

        /// Deliver a package.
        /// @param distance Distance the package has to travel.
        void DeliverPackage(unsigned dist);

        /// Prepare
        /// \param dur   Preparation takes "dur" milliseconds.
        void Prepare(unsigned int dur);

        /// Sleep
        /// \param dur   Preparation takes "dur" milliseconds.
        void Sleep(unsigned int dur);

private:
        unsigned int m_speed; ///< Delivery man speed from 1 to 10.
};
