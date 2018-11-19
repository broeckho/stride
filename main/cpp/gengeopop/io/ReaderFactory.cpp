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
#include <iostream>
#include <util/Exception.h>
#include <util/FileSys.h>

namespace gengeopop {

std::shared_ptr<CitiesReader> ReaderFactory::CreateCitiesReader(const std::string& filename)
{
        return CreateCitiesReader(stride::util::FileSys::GetDataDir() / filesys::path(filename));
}

std::shared_ptr<CitiesReader> ReaderFactory::CreateCitiesReader(const filesys::path& path)
{
        return std::make_shared<CitiesCSVReader>(OpenFile(path));
}

std::shared_ptr<CommutesReader> ReaderFactory::CreateCommutesReader(const std::string& filename)
{
        return CreateCommutesReader(stride::util::FileSys::GetDataDir() / filesys::path(filename));
}

std::shared_ptr<CommutesReader> ReaderFactory::CreateCommutesReader(const filesys::path& path)
{
        return std::make_shared<CommutesCSVReader>(OpenFile(path));
}

std::shared_ptr<HouseholdReader> ReaderFactory::CreateHouseholdReader(const std::string& filename)
{
        return CreateHouseholdReader(stride::util::FileSys::GetDataDir() / filesys::path(filename));
}

std::shared_ptr<HouseholdReader> ReaderFactory::CreateHouseholdReader(const filesys::path& path)
{
        return std::make_shared<HouseholdCSVReader>(OpenFile(path));
}

std::unique_ptr<std::istream> ReaderFactory::OpenFile(const filesys::path& path) const
{
        if (!filesys::exists(path)) {
                throw stride::util::Exception("File not found: " + path.string());
        }

        if (path.extension().string() == ".csv") {
                return std::make_unique<std::ifstream>(path.string());
        } else {
                throw stride::util::Exception("Unsupported file extension: " + path.extension().string());
        }
}

} // namespace gengeopop
