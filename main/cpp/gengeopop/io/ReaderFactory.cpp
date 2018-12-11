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

#include "ReaderFactory.h"

#include "CitiesCSVReader.h"
#include "CommutesCSVReader.h"
#include "HouseholdCSVReader.h"
#include "util/Exception.h"
#include "util/FileSys.h"

#include <iostream>

namespace gengeopop {

using namespace std;

shared_ptr<CitiesReader> ReaderFactory::CreateCitiesReader(const string& filename)
{
        return CreateCitiesReader(stride::util::FileSys::GetDataDir() / filesys::path(filename));
}

shared_ptr<CitiesReader> ReaderFactory::CreateCitiesReader(const filesys::path& path)
{
        return make_shared<CitiesCSVReader>(OpenFile(path));
}

shared_ptr<CommutesReader> ReaderFactory::CreateCommutesReader(const string& filename)
{
        return CreateCommutesReader(stride::util::FileSys::GetDataDir() / filesys::path(filename));
}

shared_ptr<CommutesReader> ReaderFactory::CreateCommutesReader(const filesys::path& path)
{
        return make_shared<CommutesCSVReader>(OpenFile(path));
}

shared_ptr<HouseholdReader> ReaderFactory::CreateHouseholdReader(const string& filename)
{
        return CreateHouseholdReader(stride::util::FileSys::GetDataDir() / filesys::path(filename));
}

shared_ptr<HouseholdReader> ReaderFactory::CreateHouseholdReader(const filesys::path& path)
{
        return make_shared<HouseholdCSVReader>(OpenFile(path));
}

unique_ptr<istream> ReaderFactory::OpenFile(const filesys::path& path) const
{
        if (!filesys::exists(path)) {
                throw stride::util::Exception("File not found: " + path.string());
        }

        if (path.extension().string() == ".csv") {
                return make_unique<ifstream>(path.string());
        } else {
                throw stride::util::Exception("Unsupported file extension: " + path.extension().string());
        }
}

} // namespace gengeopop
