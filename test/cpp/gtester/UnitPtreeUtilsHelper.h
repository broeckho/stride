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
 *  Copyright 2017, 2018 Willem L, Kuylen E, Stijven S & Broeckhove J
 */

/**
 * @file
 * Helper for unit tests of PtreeUtils.
 */

#include <string>

namespace stride {
namespace tests {

class UnitPtreeUtilsHelper
{
public:
        static std::string GetXml1()
        {
                return R"###(
<?xml version="1.0" encoding="utf-8"?>
<run>
        <time>0</time>
        <time>1</time>
        <time>2</time>
        <time>3</time>
        <time>4</time>
</run>
        )###";
        }

        static std::string GetXml2()
        {
                return R"###(
<?xml version="1.0" encoding="utf-8"?>
<run>
        <time>5</time>
        <time>6</time>
        <time>7</time>
        <time>8</time>
</run>
        )###";
        }
};

} // namespace tests
} // namespace stride
