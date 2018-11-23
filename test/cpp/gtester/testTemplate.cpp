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
 *  Copyright 2018, Niels Aerens, Thomas Avé, Jan Broeckhove, Tobia De Koninck, Robin Jadoul
 */

#include <gtest/gtest.h>

/**
 * This is a basic test file. It has a simple usage of the most frequent needed features.
 * Can be copied and adapted to create new tests quickly.
 */
namespace {

class AFixture : public testing::Test
{
protected:
        virtual void SetUp()
        {
                // Set up test environ for every case
                i = 3;
        }

        virtual void TearDown()
        {
                // Clean up after test case
        }

        int i;
};

TEST(Basic, Zero)
{
        EXPECT_EQ(1, 1);
        EXPECT_LT(1, 2);
}

TEST_F(AFixture, basicFixtureTest)
{
        // You can use the fixture here
        EXPECT_EQ(i, 3);
}

} // namespace
