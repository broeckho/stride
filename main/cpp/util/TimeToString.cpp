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
 * Utilities to tag clocks and reformat clock readout to string.
 */

#include "TimeToString.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace stride {
namespace util {

using namespace std;

string TimeToString::ToColonString(chrono::minutes d)
{
        using namespace std;
        using namespace chrono;

        ostringstream oss;
        hours         hh = duration_cast<hours>(d);
        minutes       mm = duration_cast<minutes>(d % hours(1));

        oss << right << setfill('0') << setw(2) << hh.count() << ":" << setw(2) << mm.count() << ":" << setw(2) << 0;
        return oss.str();
}

string TimeToString::ToColonString(chrono::seconds d)
{
        using namespace std;
        using namespace chrono;

        ostringstream oss;
        hours         hh = duration_cast<hours>(d);
        minutes       mm = duration_cast<minutes>(d % hours(1));
        seconds       ss = duration_cast<seconds>(d % minutes(1));

        oss << right << setfill('0') << setw(2) << hh.count() << ":" << setw(2) << mm.count() << ":" << setw(2)
            << ss.count();
        return oss.str();
}

string TimeToString::ToColonString(chrono::milliseconds d)
{
        using namespace std;
        using namespace chrono;

        ostringstream oss;
        hours         hh    = duration_cast<hours>(d);
        minutes       mm    = duration_cast<minutes>(d % hours(1));
        seconds       ss    = duration_cast<seconds>(d % minutes(1));
        milliseconds  milli = duration_cast<milliseconds>(d % seconds(1));

        oss << right << setfill('0') << setw(2) << hh.count() << ":" << setw(2) << mm.count() << ":" << setw(2)
            << ss.count() << ":" << setw(3) << milli.count();
        return oss.str();
}

string TimeToString::ToColonString(chrono::microseconds d)
{
        using namespace std;
        using namespace chrono;

        ostringstream oss;
        hours         hh    = duration_cast<hours>(d);
        minutes       mm    = duration_cast<minutes>(d % hours(1));
        seconds       ss    = duration_cast<seconds>(d % minutes(1));
        milliseconds  milli = duration_cast<milliseconds>(d % seconds(1));
        microseconds  micro = duration_cast<microseconds>(d % milliseconds(1));

        oss << right << setfill('0') << setw(2) << hh.count() << ":" << setw(2) << mm.count() << ":" << setw(2)
            << ss.count() << ":" << setw(3) << milli.count() << ":" << setw(3) << micro.count();
        return oss.str();
}

string TimeToString::ToColonString(chrono::nanoseconds d)
{
        using namespace std;
        using namespace chrono;

        ostringstream oss;
        hours         hh    = duration_cast<hours>(d);
        minutes       mm    = duration_cast<minutes>(d % hours(1));
        seconds       ss    = duration_cast<seconds>(d % minutes(1));
        milliseconds  milli = duration_cast<milliseconds>(d % seconds(1));
        microseconds  micro = duration_cast<microseconds>(d % milliseconds(1));
        nanoseconds   nano  = duration_cast<nanoseconds>(d % microseconds(1));

        oss << right << setfill('0') << setw(2) << hh.count() << ":" << setw(2) << mm.count() << ":" << setw(2)
            << ss.count() << ":" << setw(3) << milli.count() << ":" << setw(3) << micro.count() << ":" << setw(3)
            << nano.count() << endl;
        return oss.str();
}

} // namespace util
} // namespace stride
