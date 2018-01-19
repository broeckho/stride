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
 *  Copyright 2017, De Pauw J, Draulans S, Leys T, Truyts T, Van Leeuwen L
 */

#include "util/RRandom.h"

#include <trng/lagfib2plus.hpp>
#include <trng/lagfib2xor.hpp>
#include <trng/lagfib4plus.hpp>
#include <trng/lagfib4xor.hpp>
#include <trng/lcg64.hpp>
#include <trng/lcg64_shift.hpp>
#include <trng/mrg2.hpp>
#include <trng/mrg3.hpp>
#include <trng/mrg3s.hpp>
#include <trng/mrg4.hpp>
#include <trng/mrg5.hpp>
#include <trng/mrg5s.hpp>
#include <trng/yarn2.hpp>
#include <trng/yarn3.hpp>
#include <trng/yarn3s.hpp>
#include <trng/yarn4.hpp>
#include <trng/yarn5.hpp>
#include <trng/yarn5s.hpp>
#include <map>
#include <memory>
#include <string>

namespace stride {
namespace config {

/**
 * @function to create any kind of random number generator
 */
template <typename RNG>
std::shared_ptr<RNGInterface> createRNG()
{
        return std::make_shared<Random<RNG>>();
}

/**
 * @class A map of rng name to an instance of the rng
 */
class RNGMap : public std::map<std::string, std::shared_ptr<RNGInterface> (*)()>
{
public:
        RNGMap()
        {
                (*this)["trng/lcg64"] = &createRNG<trng::lcg64>;
                (*this)["trng/lcg64_shift"] = &createRNG<trng::lcg64_shift>;
                (*this)["trng/mrg2"] = &createRNG<trng::mrg2>;
                (*this)["trng/mrg3"] = &createRNG<trng::mrg3>;
                (*this)["trng/mrg4"] = &createRNG<trng::mrg4>;
                (*this)["trng/mrg5"] = &createRNG<trng::mrg5>;
                (*this)["trng/mrg3s"] = &createRNG<trng::mrg3s>;
                (*this)["trng/mrg5s"] = &createRNG<trng::mrg5s>;
                (*this)["trng/yarn2"] = &createRNG<trng::yarn2>;
                (*this)["trng/yarn3"] = &createRNG<trng::yarn3>;
                (*this)["trng/yarn4"] = &createRNG<trng::yarn4>;
                (*this)["trng/yarn5"] = &createRNG<trng::yarn5>;
                (*this)["trng/yarn3s"] = &createRNG<trng::yarn3s>;
                (*this)["trng/yarn5s"] = &createRNG<trng::yarn5s>;
                /*
                (*this)["trng/lagfib2xor"] = &createRNG<trng::lagfib2xor>;
                (*this)["trng/lagfib4xor"] = &createRNG<trng::lagfib4xor>;
                (*this)["trng/lagfib2plus"] = &createRNG<trng::lagfib2plus>;
                (*this)["trng/lagfib4plus"] = &createRNG<trng::lagfib4plus>;
                 */
        }
};

extern RNGMap rngMap;

std::string RNGvalidate(const std::string& rng_value)
{
        if (rngMap.find(rng_value) != rngMap.end()) {
                return rng_value;
        } else {
                throw std::runtime_error("Unknown RNG: " + rng_value);
        }
}

std::shared_ptr<RNGInterface> RNGget(const std::string& rng_value) { return rngMap[rng_value](); }

} // namespace config
} // namespace stride