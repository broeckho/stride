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
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for MainRunner.
 */

#include "myhayai/MainRunner.hpp"

#include "myhayai/Benchmarker.hpp"
#include "myhayai/ConsoleFileOutputter.hpp"
#include "myhayai/ConsoleOutputter.hpp"
#include "myhayai/Fixture.hpp"
#include "myhayai/JUnitXmlFileOutputter.hpp"
#include "myhayai/JUnitXmlOutputter.hpp"
#include "myhayai/JsonFileOutputter.hpp"
#include "myhayai/JsonOutputter.hpp"

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <set>
#include <vector>

#define HAYAI_VERSION "1.0.1"
#define HAYAI_MAIN_FORMAT_FLAG(_desc) myhayai::Console::TextGreen << _desc << myhayai::Console::TextDefault
#define HAYAI_MAIN_FORMAT_ARGUMENT(_desc) myhayai::Console::TextYellow << _desc << myhayai::Console::TextDefault
#define HAYAI_MAIN_FORMAT_ERROR(_desc)                                                                                 \
        myhayai::Console::TextRed << "Error:" << myhayai::Console::TextDefault << " " << _desc
#define HAYAI_MAIN_USAGE_ERROR(_desc)                                                                                  \
        {                                                                                                              \
                cerr << HAYAI_MAIN_FORMAT_ERROR(_desc) << endl << endl;                                                \
                ShowUsage(argv[0]);                                                                                    \
                return EXIT_FAILURE;                                                                                   \
        }

using namespace std;

namespace myhayai {

MainRunner::~MainRunner()
{
        // Clean up the outputters.
        for (auto it = FileOutputters.begin(); it != FileOutputters.end(); ++it) {
                delete *it;
        }
        if (StdoutOutputter) {
                delete StdoutOutputter;
        }
}

int MainRunner::ListBenchmarks()
{
        // List out the unique benchmark names.
        auto test_descriptors = Benchmarker::Instance().GetTests();
        vector<string>                testNames;
        set<string>                   uniqueTestNames;

        for (auto it = test_descriptors.begin(); it < test_descriptors.end(); ++it) {
                if (uniqueTestNames.find((*it)->CanonicalName) != uniqueTestNames.end()) {
                        continue;
                }
                testNames.push_back((*it)->CanonicalName);
                uniqueTestNames.insert((*it)->CanonicalName);
        }

        // Sort the benchmark names.
        sort(testNames.begin(), testNames.end());

        // Dump the list.
        for (auto it = testNames.begin(); it < testNames.end(); ++it) {
                cout << *it << endl;
        }

        return EXIT_SUCCESS;
}

int MainRunner::ParseArgs(int argc, char** argv, vector<char*>* residualArgs)
{
        int argI = 1;
        while (argI < argc) {
                char*  arg     = argv[argI++];
                bool   argLast = (argI == argc);
                size_t argLen  = strlen(arg);

                if (argLen == 0) {
                        continue;
                }
                // List flag.
                if ((!strcmp(arg, "-l")) || (!strcmp(arg, "--list"))) {
                        ExecutionMode = Modes::List;
                }
                // Shuffle flag.
                else if ((!strcmp(arg, "-s")) || (!strcmp(arg, "--shuffle"))) {
                        ShuffleBenchmarks = true;
                }
                // Filter flag.
                else if ((!strcmp(arg, "-f")) || (!strcmp(arg, "--filter"))) {
                        if ((argLast) || (*argv[argI] == 0))
                                HAYAI_MAIN_USAGE_ERROR(HAYAI_MAIN_FORMAT_FLAG(arg)
                                                       << " requires a pattern to be specified");
                        char* pattern = argv[argI++];
                        Benchmarker::ApplyPatternFilter(pattern);
                }
                // Output flag.
                else if ((!strcmp(arg, "-o")) || (!strcmp(arg, "--output"))) {
                        if (argLast)
                                HAYAI_MAIN_USAGE_ERROR(HAYAI_MAIN_FORMAT_FLAG(arg)
                                                       << " requires a format to be specified");
                        char* formatSpecifier = argv[argI++];
                        char* format          = formatSpecifier;
                        char* path            = strchr(formatSpecifier, ':');
                        if (path) {
                                *(path++) = 0;
                                if (!strlen(path))
                                        path = nullptr;
                        }

                        if (!strcmp(format, "console")) {
                                if (path) {
                                        FileOutputters.push_back(new ConsoleFileOutputter(path));
                                } else {
                                        if (StdoutOutputter) {
                                                delete StdoutOutputter;
                                        }
                                        StdoutOutputter = new ConsoleOutputter(cout);
                                }
                        } else if (!strcmp(format, "json"))
                                if (path) {
                                        FileOutputters.push_back(new JsonFileOutputter(path));
                                } else {
                                        if (StdoutOutputter) {
                                                delete StdoutOutputter;
                                        }
                                        StdoutOutputter = new JsonOutputter(cout);
                                }
                        else if (!strcmp(format, "junit"))
                                if (path) {
                                        FileOutputters.push_back(new JUnitXmlFileOutputter(path));
                                } else {
                                        if (StdoutOutputter) {
                                                delete StdoutOutputter;
                                        }
                                        StdoutOutputter = new JUnitXmlOutputter(cout);
                                }
                        else {
                                HAYAI_MAIN_USAGE_ERROR("invalid format: " << format);
                        }
                }
                // Console coloring flag.
                else if ((!strcmp(arg, "-c")) || (!strcmp(arg, "--color"))) {
                        if (argLast)
                                HAYAI_MAIN_USAGE_ERROR(HAYAI_MAIN_FORMAT_FLAG(arg)
                                                       << " requires an argument "
                                                       << "of either " << HAYAI_MAIN_FORMAT_FLAG("yes") << " or "
                                                       << HAYAI_MAIN_FORMAT_FLAG("no"));

                        char* choice = argv[argI++];
                        bool  enabled;
                        if ((!strcmp(choice, "yes")) || (!strcmp(choice, "true")) || (!strcmp(choice, "on")) ||
                            (!strcmp(choice, "1")))
                                enabled = true;
                        else if ((!strcmp(choice, "no")) || (!strcmp(choice, "false")) || (!strcmp(choice, "off")) ||
                                 (!strcmp(choice, "0")))
                                enabled = false;
                        else
                                HAYAI_MAIN_USAGE_ERROR("invalid argument to " << HAYAI_MAIN_FORMAT_FLAG(arg) << ": "
                                                                              << choice);

                        Console::SetFormattingEnabled(enabled);
                }
                // Help.
                else if ((!strcmp(arg, "-?")) || (!strcmp(arg, "-h")) || (!strcmp(arg, "--help"))) {
                        ShowUsage(argv[0]);
                        return EXIT_FAILURE;
                } else if (residualArgs) {
                        residualArgs->push_back(arg);
                } else {
                        HAYAI_MAIN_USAGE_ERROR("unknown option: " << arg);
                }
        }

        return EXIT_SUCCESS;
}

int MainRunner::Run()
{
        int ret = EXIT_FAILURE;
        // Execute based on the selected mode.
        switch (ExecutionMode) {
        case Modes::Run: ret = RunBenchmarks(); break;
        case Modes::List: ret = ListBenchmarks(); break;
        }
        return ret;
}

int MainRunner::RunBenchmarks()
{
        // Hook up the outputs.
        if (StdoutOutputter)
                Benchmarker::AddOutputter(*StdoutOutputter);

        for (auto it = FileOutputters.begin(); it < FileOutputters.end(); ++it) {
                FileOutputter& fileOutputter = **it;
                try {
                        fileOutputter.SetUp();
                } catch (exception& e) {
                        cerr << HAYAI_MAIN_FORMAT_ERROR(e.what()) << endl;
                        return EXIT_FAILURE;
                }
                Benchmarker::AddOutputter(fileOutputter.GetOutputter());
        }

        // Run the benchmarks.
        if (ShuffleBenchmarks) {
                srand(static_cast<unsigned>(time(0)));
                Benchmarker::ShuffleTests();
        }
        Benchmarker::RunAllTests();

        return EXIT_SUCCESS;
}

void MainRunner::ShowUsage(const char* execName)
{
        const char* baseName = strrchr(execName, '/');

        cerr << "Usage: " << (baseName ? baseName + 1 : execName) << " " << HAYAI_MAIN_FORMAT_FLAG("[OPTIONS]") << endl
             << endl

             << "  Runs the benchmarks for this project." << endl
             << endl

             << "Benchmark selection options:" << endl
             << "  " << HAYAI_MAIN_FORMAT_FLAG("-l") << ", " << HAYAI_MAIN_FORMAT_FLAG("--list") << endl
             << "    List the names of all benchmarks instead of "
             << "running them." << endl
             << "  " << HAYAI_MAIN_FORMAT_FLAG("-f") << ", " << HAYAI_MAIN_FORMAT_FLAG("--filter") << " <"
             << HAYAI_MAIN_FORMAT_ARGUMENT("pattern") << ">" << endl
             << "    Run only the tests whose name matches one of the "
             << "positive patterns but" << endl
             << "    none of the negative patterns. '?' matches any "
             << "single character; '*'" << endl
             << "    matches any substring; ':' separates two "
             << "patterns." << endl

             << "Benchmark execution options:" << endl
             << "  " << HAYAI_MAIN_FORMAT_FLAG("-s") << ", " << HAYAI_MAIN_FORMAT_FLAG("--shuffle") << endl
             << "    Randomize benchmark execution order." << endl
             << endl

             << "Benchmark output options:" << endl
             << "  " << HAYAI_MAIN_FORMAT_FLAG("-o") << ", " << HAYAI_MAIN_FORMAT_FLAG("--output") << " <"
             << HAYAI_MAIN_FORMAT_ARGUMENT("format") << ">[:" << HAYAI_MAIN_FORMAT_ARGUMENT("<path>") << "]" << endl
             << "    Output results in a specific format. If no "
             << "path is specified, the output" << endl
             << "    will be presented on stdout. Can be specified "
             << "multiple times to get output" << endl
             << "    in different formats. The supported formats are:" << endl
             << endl
             << "    " << HAYAI_MAIN_FORMAT_ARGUMENT("console") << endl
             << "      Standard console output." << endl
             << "    " << HAYAI_MAIN_FORMAT_ARGUMENT("json") << endl
             << "      JSON." << endl
             << "    " << HAYAI_MAIN_FORMAT_ARGUMENT("junit") << endl
             << "      JUnit-compatible XML (very restrictive.)" << endl
             << endl
             << "    If multiple output formats are provided without "
             << "a path, only the last" << endl
             << "    provided format will be output to stdout." << endl
             << "  " << HAYAI_MAIN_FORMAT_FLAG("--c") << ", " << HAYAI_MAIN_FORMAT_FLAG("--color") << " ("
             << Console::TextGreen << "yes" << Console::TextDefault << "|" << Console::TextGreen << "no"
             << Console::TextDefault << ")" << endl
             << "    Enable colored output when available. Default " << Console::TextGreen << "yes"
             << Console::TextDefault << "." << endl
             << endl

             << "Miscellaneous options:" << endl
             << "  " << HAYAI_MAIN_FORMAT_FLAG("-?") << ", " << HAYAI_MAIN_FORMAT_FLAG("-h") << ", "
             << HAYAI_MAIN_FORMAT_FLAG("--help") << endl
             << "    Show this help information." << endl
             << endl

             << "hayai version: " << HAYAI_VERSION << endl
             << "Clock implementation: " << Clock::Description() << endl;
}

} // namespace myhayai

#undef HAYAI_MAIN_FORMAT_FLAG
#undef HAYAI_MAIN_FORMAT_ARGUMENT
#undef HAYAI_MAIN_FORMAT_ERROR
#undef HAYAI_MAIN_USAGE_ERROR
