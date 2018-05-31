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
 * Unit tests of Csv.
 */

#include "util/CSV.h"

#include <gtest/gtest.h>

using namespace stride::util;

TEST(UnitCsvTest, SafeCastTest)
{
        EXPECT_THROW(safe_cast<int>("10test"), boost::bad_lexical_cast);
        EXPECT_THROW(safe_cast<int>("abc"), boost::bad_lexical_cast);
        EXPECT_THROW(safe_cast<unsigned int>("-100"), boost::bad_numeric_cast);
        EXPECT_THROW(safe_cast<uint8_t>("1024"), boost::bad_numeric_cast);
        EXPECT_THROW(safe_cast<double>("10.10.1"), boost::bad_lexical_cast);

        EXPECT_EQ(safe_cast<int>("10"), 10);
        EXPECT_EQ(safe_cast<unsigned int>("100"), 100);
        EXPECT_EQ(safe_cast<uint8_t>("125"), 125);
        EXPECT_EQ(safe_cast<double>("100.10"), 100.10);
        EXPECT_EQ(safe_cast<std::string>(std::string("abc")), "abc");
}

TEST(UnitCsvTest, TestBadCast)
{
        std::string csvString = "id,naam,col3,col4,col5\n"
                                "10test,abc,-100,1024,100.10.1\n"
                                "10,abc,100,125,100.10";

        std::istringstream instream(csvString);

        CSV reader(instream);

        const auto& row1 = reader.begin();
        EXPECT_THROW(row1->GetValue<int>("id"), boost::bad_lexical_cast);
        EXPECT_THROW(row1->GetValue<int>("naam"), boost::bad_lexical_cast);
        EXPECT_THROW(row1->GetValue<unsigned int>("col3"), boost::bad_numeric_cast);
        EXPECT_THROW(row1->GetValue<uint8_t>("col4"), boost::bad_numeric_cast);
        EXPECT_THROW(row1->GetValue<double>("col5"), boost::bad_lexical_cast);

        const auto& row2 = reader.begin() + 1;
        EXPECT_EQ(row2->GetValue<int>("id"), 10);
        EXPECT_EQ(row2->GetValue("naam"), "abc");
        EXPECT_EQ(row2->GetValue<unsigned int>("col3"), 100);
        EXPECT_EQ(row2->GetValue<uint8_t>("col4"), 125);
        EXPECT_EQ(row2->GetValue<double>("col5"), 100.10);
}
