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
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 */
/**
 * @file
 * Implementation file for myhayai::CliContoller.
 */

#include "MyhayaiController.h"

#include "BenchControlHelper.h"
#include "BenchmarkRunner.h"
#include "Console.h"
#include "ConsoleViewer.h"
#include "PtreeViewer.h"

#include <boost/property_tree/info_parser.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <tclap/CmdLine.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <locale>
#include <random>
#include <regex>
#include <stdexcept>

using namespace std;
using namespace boost::property_tree;
using namespace boost::property_tree::xml_parser;
using namespace TCLAP;

namespace myhayai {

void MyhayaiController::Control()
{
        if (m_list_mode) {
                return ListBenchmarks();
        } else {
                return RunBenchmarks();
        }
}

void MyhayaiController::ListBenchmarks() const
{
        BenchmarkRunner&   runner = BenchmarkRunner::Instance();
        BenchControlHelper helper(runner.GetTestDescriptors(), m_negative, m_positive);

        using namespace console;
        unsigned int incl_en  = 0U;
        unsigned int incl_dis = 0U;
        unsigned int excl_en  = 0U;
        unsigned int excl_dis = 0U;

        if (m_no_color) {
                cout << console::Color::DisableColor;
        } else {
                cout << console::Color::EnableColor;
        }

        cout << Color::Green << "[==========]" << Color::Default << " BenchmarkRunner listing tests." << endl;
        for (const auto& item : BenchmarkRunner::Instance().GetTestDescriptors()) {
                const auto& t_n = item.first;
                if (helper.IsIncluded(t_n)) {
                        if (item.second.m_is_disabled) {
                                ++incl_dis;
                                cout << Color::Cyan << "[ INCLUDED/DISABLED ]  " << Color::Default << t_n << endl;
                        } else {
                                ++incl_en;
                                cout << Color::Green << "[ INCLUDED/ENABLED  ]  " << Color::Default << t_n << endl;
                        }
                } else {
                        if (item.second.m_is_disabled) {
                                ++excl_dis;
                                cout << Color::Purple << "[ EXCLUDED/DISABLED ]  " << Color::Default << t_n << endl;
                        } else {
                                ++excl_en;
                                cout << Color::Red << "[ EXCLUDED/ENABLED  ]  " << Color::Default << t_n << endl;
                        }
                }
        }
        cout << Color::Green << "[==========]" << Color::Default << " Included: " << incl_en << " enabled, " << incl_dis
             << " disabled." << endl;
        cout << Color::Green << "[==========]" << Color::Default << " Excluded: " << excl_en << " enabled; " << excl_dis
             << " disabled." << endl;
}

void MyhayaiController::ParseArgs(int argc, char** argv)
{
        try {
                CmdLine cmd("myhayai_demo", ' ', "1.0");

                SwitchArg noColorArg("", "no_color", "Do not use color on console output.", cmd, false);

                vector<string>           execs{"on", "sim"};
                ValuesConstraint<string> vc(execs);
                ValueArg<string>         exec("", "exec", "Execute the specified function.", false, "sim", &vc, cmd);

                string           sf("Produce output in info format in file at <path>.");
                ValueArg<string> infoArg("", "info_path", sf, false, "", "path", cmd);

                string           sj("Produce output in json format in file at <path>.");
                ValueArg<string> jsonArg("", "json_path", sj, false, "", "path", cmd);

                //
                string           sx("Produce output in xml format in file at <path>.");
                ValueArg<string> xmlArg("", "xml_path", sx, false, "btester.xml", "path", cmd);

                string           sp("Positive regex for the canonical names (<group_name>.<test_name>) "
                          "in the current benchmarking run. A test gets included if its name "
                          "partially matches at least one of the positive regexes and none of "
                          "negative regexes. If no positive regexes are defined all name are "
                          "considerd to match a positive regex. If no negative regexes are"
                          "defined are considered to match a negative regex.");
                MultiArg<string> positiveArg("p", "positive_regex", sp, false, "<regex>", cmd);

                string           sn("Negative regex for the canonical names (<group_name>.<test_name>) "
                          "in the current benchmarking run. A test gets included if its name "
                          "partially matches at least one of the positive regexes and none of "
                          "negative regexes. If no positive regexes are defined all name are "
                          "considerd to match a positive regex. If no negative regexes are"
                          "defined are considered to match a negative regex.");
                MultiArg<string> negativeArg("n", "negative_regex", sn, false, "<regex>", cmd);

                SwitchArg shuffleArg("s", "shuffle", "Shuffle the execution order of the tests.", cmd, false);

                SwitchArg listArg("l", "list", "List benchmarking tests (i.o executing them).", cmd, false);

                //
                cmd.parse(argc, static_cast<const char* const*>(argv));
                m_list_mode = listArg.getValue();
                m_info_path = infoArg.getValue();
                m_json_path = jsonArg.getValue();
                m_xml_path  = xmlArg.getValue();
                m_no_color  = noColorArg.getValue();
                m_negative  = negativeArg.getValue();
                m_positive  = positiveArg.getValue();
                m_shuffle   = shuffleArg.getValue();
        } catch (ArgException& e) {
                cerr << "MyhayaiController::ParseArgs>" << e.what();
                throw runtime_error(e.what()); /// do not want main.cpp to bother with specific exception classes.
        }
}

void MyhayaiController::RunBenchmarks()
{
        BenchmarkRunner&   runner = BenchmarkRunner::Instance();
        BenchControlHelper helper(runner.GetTestDescriptors(), m_negative, m_positive);

        // Get the the canonical test names, after applying include/exclude regex and shuffle.
        auto names = helper.GetIncludedNames();
        if (m_shuffle) {
                helper.Shuffle(names);
        }

        // Always the console viewer.
        using cv_t = ConsoleViewer<chrono::milliseconds>;
        auto cv    = make_shared<cv_t>(cout, m_no_color);
        runner.Register(cv, bind(&cv_t::Update, cv, placeholders::_1));

        // Possibly the ptree viewer.
        using pv_t = PtreeViewer<chrono::milliseconds>;
        auto pv    = make_shared<pv_t>();
        if (!(m_info_path.empty() && m_json_path.empty() && m_xml_path.empty())) {
                runner.Register(pv, bind(&pv_t::Update, pv, placeholders::_1));
        }

        // Run them.
        runner.RunTests(names);

        // Possibly the ptree viewer.
        if (!m_info_path.empty()) {
                write_info(m_info_path, pv->CGet());
        }
        if (!m_xml_path.empty()) {
                write_json(m_xml_path, pv->CGet());
        }
        if (!m_xml_path.empty()) {
                write_xml(m_xml_path, pv->CGet(), std::locale(), xml_writer_make_settings<ptree::key_type>(' ', 8));
        }
}

} // namespace myhayai
