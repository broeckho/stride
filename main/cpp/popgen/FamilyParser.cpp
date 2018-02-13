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
 * Parser for family data implementation.
 */

#include "FamilyParser.h"
#include "util/InstallDirs.h"

using namespace stride;
using namespace popgen;
using namespace std;
using namespace util;

vector<FamilyConfig> FamilyParser::parseFamilies(string filename) const
{
        // Precond: datdir OK
        vector<FamilyConfig> result;
        string               line;
        ifstream             myfile((InstallDirs::GetDataDir() /= filename).string());
        if (myfile.is_open()) {
                while (getline(myfile, line)) {
                        if (line != "") {
                                result.push_back(parseFamily(line));
                        }
                }
                myfile.close();
        } else {
                throw invalid_argument("In FamilyParser: Invalid file name");
        }

        return result;
}

FamilyConfig FamilyParser::parseFamily(string config) const
{
        FamilyConfig result;

        for (char& c : config) {
                static uint current_age       = 0;
                static bool encountered_space = false;
                if (c == ' ') {
                        if (not encountered_space) {
                                result.push_back(current_age);
                        }
                        encountered_space = true;
                        current_age       = 0;
                } else if (&c == &config.back()) {
                        encountered_space = false;
                        current_age *= 10;
                        current_age += uint(c) - uint('0');
                        result.push_back(current_age);
                        current_age = 0;
                } else if (c >= int('0') && c <= int('9')) {
                        encountered_space = false;
                        current_age *= 10;
                        current_age += uint(c) - uint('0');
                } else {
                        throw invalid_argument(
                            "In FamilyParser: Encountered something that isn't an unsigned integer or a space");
                }
        }

        return result;
}