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
 * Implementation of RnPcg.
 */

#include "RnMan.h"
#include "Rn.h"
#include "StringUtils.h"

#include <trng/discrete_dist.hpp>
#include <trng/lcg64.hpp>
#include <trng/uniform01_dist.hpp>
#include <trng/uniform_int_dist.hpp>
#include <array>
#include <cctype>
#include <functional>
#include <pcg/pcg_random.hpp>
#include <randutils/randutils.hpp>

using namespace std;
using namespace randutils;

namespace stride {
namespace util {

// The construction below lets you choose (at compile time)
// between the pcg64 and trng::lcg64 random engines. It's a hacky
// construct but required because the forward class definition
// of RnEgine in RnMan.h cannot be combined with a using
// statement here. A macro to generate the class definition
// would make it more scaleable, but since we have only two ...

// If you want to use the pcg64 random engine, uncomment the
// class definition here and keep the one below commented out.
class RnEngine : public Rn<pcg64>
{
        using Rn<pcg64>::Rn;
};

// If you want to use the trng::lcg64 random engine, uncomment the
// class definition here and keep the one above commented out.
/*
class RnLcg64 : public Rn<trng::lcg64>
{
        using Rn<trng::lcg64>::Rn;
};
*/

RnMan::RnMan() : m_rn(make_shared<RnEngine>()) {}

RnMan::RnMan(const RnInfo& info) : m_rn(make_shared<RnEngine>(info)) {}

bool RnMan::operator==(const RnMan& other) { return *m_rn == *(other.m_rn); }

RnInfo RnMan::GetInfo() const { return m_rn->GetInfo(); }

std::function<double()> RnMan::GetUniform01Generator(unsigned int i) { return m_rn->GetUniform01Generator(i); }

std::function<int()> RnMan::GetUniformIntGenerator(int a, int b, unsigned int i)
{
        return m_rn->GetUniformIntGenerator(a, b, i);
}

std::function<int()> RnMan::GetDiscreteGenerator(const vector<double>& weights, unsigned int i)
{
        return m_rn->GetDiscreteGenerator(weights.begin(), weights.end(), i);
}

bool RnMan::MakeWeightedCoinFlip(double fraction, unsigned int i)
{
        array<double, 2> weights{ 1.0 - fraction, fraction};
        // -> 0, return is false -> not part of the fraction
        // -> 1, return is true -> part of the fraction
        auto dist = m_rn->GetDiscreteGenerator(weights.begin(), weights.end(), i);
        return static_cast<bool>(dist());
}

void RnMan::Initialize(const RnInfo& info) { m_rn->Initialize(info); }

bool RnMan::IsEmpty() const { return m_rn->IsEmpty(); }

void RnMan::Shuffle(vector<unsigned int>& indices, unsigned int i) { return m_rn->Shuffle(indices, i); }

} // namespace util
} // namespace stride
