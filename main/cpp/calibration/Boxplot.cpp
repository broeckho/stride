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

#include "Boxplot.h"

#include <algorithm>

namespace calibration {

/*
 * Inspired by https://doc.qt.io/qt-5.10/qtcharts-boxplotchart-example.html
 * */

Boxplot::Boxplot() : m_logger(stride::util::LogUtils::CreateCliLogger("boxplot_m_logger", "boxplot_log.txt")) {}

double Boxplot::FindMedian(unsigned long begin, unsigned long end, std::vector<unsigned int>& results) const
{
        unsigned long count = end - begin;
        if (count % 2) {
                return results.at(count / 2 + begin);
        } else {
                double right = results.at(count / 2 + begin);
                double left  = results.at(count / 2 - 1 + begin);
                return (right + left) / 2.0;
        }
}
std::vector<BoxplotData> Boxplot::Calculate(const std::map<std::string, std::vector<std::vector<unsigned int>>>& data,
                                            unsigned int step) const
{
        std::vector<BoxplotData> boxplots;
        for (const auto& config : data) {
                std::vector<unsigned int> results;
                for (const auto& run : config.second) {
                        if (run.size() - 1 < step) {
                                m_logger->error("No such step {} for {} in input.", step, config.first);
                                continue;
                        }
                        results.push_back(run[step]);
                }
                boxplots.push_back(Calculate(results, config.first + " step " + std::to_string(step)));
        }
        return boxplots;
}
std::vector<BoxplotData> Boxplot::CalculateLastStep(
    const std::map<std::string, std::vector<std::vector<unsigned int>>>& data) const
{
        std::vector<BoxplotData> boxplots;
        for (const auto& config : data) {
                std::vector<unsigned int> results;
                for (const auto& run : config.second) {
                        results.push_back(run[run.size() - 1]);
                }
                boxplots.push_back(Calculate(results, config.first));
        }
        return boxplots;
}

std::vector<BoxplotData> Boxplot::Calculate(
    const std::map<std::string, std::vector<std::vector<unsigned int>>>& data) const
{
        std::vector<BoxplotData> boxplots;
        for (const auto& config : data) {
                if (config.second.empty())
                        continue;
                for (unsigned int step = 0; step < config.second[0].size(); step++) {
                        std::vector<unsigned int> results;
                        for (const auto& run : config.second) {
                                if (run.size() - 1 < step) {
                                        m_logger->error(
                                            "Inconstistent input data! Not all runs have the same number of steps.");
                                        continue;
                                }
                                results.push_back(run[step]);
                        }
                        boxplots.push_back(Calculate(results, config.first + " step " + std::to_string(step)));
                }
        }
        return boxplots;
}

BoxplotData Boxplot::Calculate(std::vector<unsigned int> data, std::string name) const
{
        std::sort(data.begin(), data.end());
        if (data.size() <= 1) {
                m_logger->warn("Warning: Could not generate information: not enough data.");
        }
        unsigned int count = data.size();
        BoxplotData  boxplotData;
        boxplotData.name           = name;
        boxplotData.min_value      = *std::min_element(data.begin(), data.end());
        boxplotData.max_value      = *std::max_element(data.begin(), data.end());
        boxplotData.median         = FindMedian(0, count, data);
        boxplotData.lower_quartile = FindMedian(0, static_cast<unsigned long>(count / 2.0), data);
        boxplotData.upper_quartile = FindMedian(static_cast<unsigned long>(count / 2.0 + (count % 2)), count, data);
        return boxplotData;
}

} // namespace calibration
