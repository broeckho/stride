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

#include "Calibrator.h"
#include "../../test/cpp/gtester/ScenarioData.h"
#include <pop/Population.h>
#include <sim/Sim.h>
#include <sim/SimRunner.h>
#include <viewers/InfectedViewer.h>

#include <boost/property_tree/json_parser.hpp>
#include <fstream>
#include <iostream>
#include <numeric>
#include <omp.h>
#include <random>
#include <utility>
#include <set>

namespace calibration {

Calibrator::Calibrator() : logger(stride::util::LogUtils::CreateCliLogger("stride_logger", "calibration_log.txt")) {}

std::map<std::string, std::vector<unsigned int>> Calibrator::RunSingle(
    const std::vector<std::pair<boost::property_tree::ptree, std::string>>& configs) const
{
        std::map<std::string, std::vector<unsigned int>> results;
        logger->info("Starting to compute exact values for testcases");

        spdlog::register_logger(logger);

#pragma omp parallel for
        for (unsigned int config = 0; config < configs.size(); config++) {
                stride::util::RnMan rn_manager;
                rn_manager.Initialize(
                    stride::util::RnMan::Info{configs[config].first.get<std::string>("run.rng_seed", "1,2,3,4"), "",
                                              configs[config].first.get<unsigned int>("run.num_threads")});
                auto pop    = stride::Population::Create(configs[config].first, rn_manager);
                auto runner = std::make_shared<stride::SimRunner>(configs[config].first, pop, rn_manager);
                std::shared_ptr<stride::viewers::InfectedViewer> infectedViewer =
                    std::make_shared<stride::viewers::InfectedViewer>(runner);
                runner->Register(infectedViewer,
                                 bind(&stride::viewers::InfectedViewer::Update, infectedViewer, std::placeholders::_1));
                runner->Run();
                results[configs[config].second] = infectedViewer->GetInfectionCounts();
        }
        return results;
}

std::map<std::string, std::vector<std::vector<unsigned int>>> Calibrator::RunMultiple(
    unsigned int count, const std::vector<std::pair<boost::property_tree::ptree, std::string>>& configs) const
{
        std::map<std::string, std::vector<std::vector<unsigned int>>> results;

        std::random_device rd;

        std::vector<std::random_device::result_type> seeds;

        for (unsigned int i = 0; i < count * configs.size(); i++) {
                seeds.push_back(rd());
        }

        for (const auto& config : configs) {
                results[config.second].resize(count);
        }
#pragma omp parallel for collapse(2)
        for (unsigned int config = 0; config < configs.size(); config++) {
                for (unsigned int i = 0; i < count; i++) {
                        auto tag       = configs[config].second;
                        auto config_pt = configs[config].first;
                        auto seed      = seeds[config * count + i];

                        config_pt.put("run.rng_seed", seed);
                        logger->info("Starting the testcase {}, run {} of {} using seed {}", tag, i, count, seed);
                        stride::util::RnMan rn_manager;
                        rn_manager.Initialize(
                            stride::util::RnMan::Info{config_pt.get<std::string>("run.rng_seed", "1,2,3,4"), "",
                                                      config_pt.get<unsigned int>("run.num_threads")});
                        auto pop    = stride::Population::Create(config_pt, rn_manager);
                        auto runner = std::make_shared<stride::SimRunner>(config_pt, pop, rn_manager);
                        std::shared_ptr<stride::viewers::InfectedViewer> infectedViewer =
                            std::make_shared<stride::viewers::InfectedViewer>(runner);
                        runner->Register(infectedViewer, bind(&stride::viewers::InfectedViewer::Update, infectedViewer,
                                                              std::placeholders::_1));
                        runner->Run();

                        // Get the infected count
                        const unsigned int res = runner->GetSim()->GetPopulation()->GetInfectedCount();
                        logger->info("Finished running testcase {}, {} people were infected", tag, res);
                        results[tag][i] = infectedViewer->GetInfectionCounts();
                }
        }
        return results;
}

void Calibrator::PrintStep(const std::vector<unsigned int>& results, std::string tag, unsigned int step) const
{
        auto p = FindMeanStdev(results);
        logger->info("Found mean {} and standard deviation {} for testcase {} at step {}", p.first, p.second, tag,
                     step);
}

std::pair<double, double> Calibrator::FindMeanStdev(std::vector<unsigned int> results) const
{
        double sum  = std::accumulate(results.begin(), results.end(), 0.0);
        double mean = sum / results.size();

        // Inspired by https://stackoverflow.com/a/7616783
        std::vector<double> diff(results.size());
        std::transform(results.begin(), results.end(), diff.begin(), [mean](double x) { return x - mean; });
        double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double stdev  = std::sqrt(sq_sum / results.size());
        return std::make_pair(mean, stdev);
}

void Calibrator::PrintMultipleResultsAtStep(
    const std::map<std::string, std::vector<std::vector<unsigned int>>>& results, unsigned int step) const
{
        for (auto& result_pair : results) {
                std::vector<unsigned int> data;
                for (const auto& run : result_pair.second) {
                        if (run.size() - 1 < step) {
                                logger->error("This no such step in the simulation: {}", step);
                                continue;
                        }
                        data.push_back(run[step]);
                }
                PrintStep(data, result_pair.first, step);
        }
}

void Calibrator::PrintSingleResults(const std::map<std::string, std::vector<unsigned int>>& results) const
{
        for (const auto& config : results) {
                logger->info("Found exact value for testcase {}: {}", config.first, *config.second.rbegin());
        }
}

void Calibrator::PrintSingleResultsAtStep(const std::map<std::string, std::vector<unsigned int>>& results,
                                          unsigned int                                            step) const
{
        for (const auto& config : results) {
                logger->info("Found exact value for testcase {}: {} at step {}", config.first, config.second[step],
                             step);
        }
}

void Calibrator::PrintMultipleResults(
    const std::map<std::string, std::vector<std::vector<unsigned int>>>& results) const
{
        for (auto& result_pair : results) {
                if (result_pair.second.empty())
                        continue;
                std::vector<unsigned int> data;
                for (const auto& run : result_pair.second) {
                        if (run.empty())
                                continue;
                        data.push_back(*run.rbegin());
                }
                PrintStep(data, result_pair.first, result_pair.second[0].size() - 1);
        }
}
void Calibrator::WriteResults(const std::map<std::string, std::vector<unsigned int>>&              single,
                              const std::map<std::string, std::vector<std::vector<unsigned int>>>& multiple,
                              std::string                                                          filename) const
{
        boost::property_tree::ptree root;
        std::set<std::string>       tags;

        for (const auto& tag : single)
                tags.insert(tag.first);
        for (const auto& tag : multiple)
                tags.insert(tag.first);

        for (const auto& tag : tags) {
                boost::property_tree::ptree tags_root;
                unsigned int                size;
                if (multiple.count(tag) && !multiple.at(tag).empty())
                        size = multiple.at(tag)[0].size();
                else if (single.count(tag) && !single.at(tag).empty())
                        size = single.at(tag).size();
                else
                        continue;
                for (unsigned int step = 0; step < size; step++) {
                        boost::property_tree::ptree step_root;
                        if (multiple.count(tag)) {
                                std::vector<unsigned int>   data;
                                boost::property_tree::ptree results;
                                for (const auto& vect : multiple.at(tag)) {
                                        if (vect.size() - 1 < step)
                                                continue;
                                        data.push_back(vect[step]);
                                        boost::property_tree::ptree result;
                                        result.put("", vect[step]);
                                        results.push_back(std::make_pair("", result));
                                }
                                auto p = FindMeanStdev(data);
                                step_root.put("mean", p.first);
                                step_root.put("standard deviation", p.second);
                                step_root.add_child("data", results);
                        }
                        if (single.count(tag)) {
                                step_root.put("exact", single.at(tag)[step]);
                        }
                        tags_root.add_child(std::to_string(step), step_root);
                }
                root.add_child(tag, tags_root);
        }
        std::ofstream file;
        file.open(filename);
        if (!file.good()) {
                logger->error("There was a problem creating the file '{}'", filename);
                return;
        }
        boost::property_tree::write_json(file, root);
}

void Calibrator::WriteMultipleResults(const std::map<std::string, std::vector<std::vector<unsigned int>>>& results,
                                      std::string filename) const
{
        WriteResults(std::map<std::string, std::vector<unsigned int>>(), results, filename);
}

void Calibrator::WriteSingleResults(const std::map<std::string, std::vector<unsigned int>>& results,
                                    std::string                                             filename) const
{
        WriteResults(results, std::map<std::string, std::vector<std::vector<unsigned int>>>(), filename);
}

} // namespace calibration
