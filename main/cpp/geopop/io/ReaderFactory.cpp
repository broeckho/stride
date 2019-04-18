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
 *  Copyright 2018, Jan Broeckhove and Bistromatics group.
 */

#include "ReaderFactory.h"

#include "CommutesCSVReader.h"
#include "HouseholdCSVReader.h"
#include "LocationsCSVReader.h"
#include "util/FileSys.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

namespace geopop {

using namespace std;
using namespace stride::util;

shared_ptr<LocationsReader> ReaderFactory::CreateLocationsReader(const string &filename)
{
        return CreateLocationsReader(FileSys::GetDataDir() / filesys::path(filename));
}

shared_ptr<LocationsReader> ReaderFactory::CreateLocationsReader(const filesys::path &path)
{
        return make_shared<LocationsCSVReader>(OpenFile(path));
}

std::shared_ptr<CommutesReader> ReaderFactory::CreateCommutesReader(const std::string& filename)
{
        return CreateCommutesReader(FileSys::GetDataDir() / filesys::path(filename));
}

shared_ptr<CommutesReader> ReaderFactory::CreateCommutesReader(const filesys::path& path)
{
        return make_shared<CommutesCSVReader>(OpenFile(path));
}

std::shared_ptr<HouseholdReader> ReaderFactory::CreateHouseholdReader(const std::string& filename)
{
        return CreateHouseholdReader(FileSys::GetDataDir() / filesys::path(filename));
}

shared_ptr<HouseholdReader> ReaderFactory::CreateHouseholdReader(const filesys::path& path)
{
        return make_shared<HouseholdCSVReader>(OpenFile(path));
}

std::unique_ptr<std::istream> ReaderFactory::OpenFile(const filesys::path& path)
{
        if (!filesys::exists(path)) {
                throw runtime_error("File not found: " + path.string());
        }

        if (path.extension().string() == ".csv") {
                return make_unique<ifstream>(path.string());
        } else {
                throw runtime_error("Unsupported file extension: " + path.extension().string());
        }
}

} // namespace geopop
