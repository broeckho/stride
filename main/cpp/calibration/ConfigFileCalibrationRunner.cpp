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

#include "ConfigFileCalibrationRunner.h"
#include "../../test/cpp/gtester/ScenarioData.h"
#include "Boxplot.h"
#include "Calibrator.h"
#include "util/FileSys.h"
#include "util/RunConfigManager.h"

#include <regex>

#ifdef BOOST_FOUND
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
namespace filesys = boost::filesystem;
#else
#include <filesystem>
namespace filesys = std::filesystem;
#endif

namespace calibration {

ConfigFileCalibrationRunner::ConfigFileCalibrationRunner(std::vector<std::string> configFiles) : CalibrationRunner()
{
        for (auto config : configFiles) {
                boost::property_tree::ptree configPt;

                if (regex_search(config, std::regex("^name="))) {
                        config   = regex_replace(config, std::regex(std::string("^name=")), std::string(""));
                        configPt = stride::util::RunConfigManager::Create(config);
                } else {
                        config = regex_replace(config, std::regex(std::string("^file=")), std::string(""));
                        const filesys::path configPath = stride::util::FileSys::GetConfigDir() / config;
                        configPt                          = stride::util::FileSys::ReadPtreeFile(configPath);
                }
                configs.emplace_back(configPt, config);
        }
}

} // namespace calibration
