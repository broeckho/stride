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
 *  The original copyright, to be found in the directory one level higher
 *  still aplies.
 */
/**
 * @file
 * Implementation file for JsonOutputter.
 */

#include "myhayai/JsonOutputter.hpp"

#include <iomanip>
#include <ostream>

#define JSON_OBJECT_BEGIN "{"
#define JSON_OBJECT_END "}"
#define JSON_ARRAY_BEGIN "["
#define JSON_ARRAY_END "]"
#define JSON_STRING_BEGIN "\""
#define JSON_STRING_END "\""
#define JSON_NAME_SEPARATOR ":"
#define JSON_VALUE_SEPARATOR ","
#define JSON_TRUE "true"
#define JSON_FALSE "false"

namespace myhayai {

// const char* JSON_OBJECT_BEGIN = "{";

void JsonOutputter::Begin(const std::size_t&, const std::size_t&)
{
        _stream << JSON_OBJECT_BEGIN

                JSON_STRING_BEGIN "format_version" JSON_STRING_END JSON_NAME_SEPARATOR "1"

            JSON_VALUE_SEPARATOR

                JSON_STRING_BEGIN "benchmarks" JSON_STRING_END JSON_NAME_SEPARATOR JSON_ARRAY_BEGIN;
}

void JsonOutputter::End(const std::size_t&, const std::size_t&) { _stream << JSON_ARRAY_END JSON_OBJECT_END; }

void JsonOutputter::BeginTest(const std::string& fixtureName, const std::string& testName,
                              const TestParametersDescriptor& parameters, const std::size_t& runsCount)
{
        BeginTestObject(fixtureName, testName, parameters, runsCount, false);
}

void JsonOutputter::SkipDisabledTest(const std::string& fixtureName, const std::string& testName,
                                     const TestParametersDescriptor& parameters, const std::size_t& runsCount)
{
        BeginTestObject(fixtureName, testName, parameters, runsCount, true);
        EndTestObject();
}

void JsonOutputter::EndTest(const std::string& fixtureName, const std::string& testName,
                            const TestParametersDescriptor& parameters, const TestResult& result)
{
        (void)fixtureName;
        (void)testName;
        (void)parameters;

        _stream << JSON_VALUE_SEPARATOR

                JSON_STRING_BEGIN "runs" JSON_STRING_END JSON_NAME_SEPARATOR JSON_ARRAY_BEGIN;

        const std::vector<uint64_t>& runTimes = result.RunTimes();

        for (auto it = runTimes.cbegin(); it != runTimes.cend(); ++it) {
                if (it != runTimes.begin())
                        _stream << JSON_VALUE_SEPARATOR;

                _stream << JSON_OBJECT_BEGIN JSON_STRING_BEGIN "duration" JSON_STRING_END JSON_NAME_SEPARATOR
                        << std::fixed << std::setprecision(6) << (double(*it) / 1000000.0) << JSON_OBJECT_END;
        }

        _stream << JSON_ARRAY_END;

        WriteDoubleProperty("mean", result.RunTimeAverage());
        WriteDoubleProperty("std_dev", result.RunTimeStdDev());
        WriteDoubleProperty("median", result.RunTimeMedian());
        WriteDoubleProperty("quartile_1", result.RunTimeQuartile1());
        WriteDoubleProperty("quartile_3", result.RunTimeQuartile3());

        EndTestObject();
}

void JsonOutputter::BeginTestObject(const std::string& fixtureName, const std::string& testName,
                                    const TestParametersDescriptor& parameters, const std::size_t&, bool disabled)
{
        if (_firstTest)
                _firstTest = false;
        else
                _stream << JSON_VALUE_SEPARATOR;

        _stream << JSON_OBJECT_BEGIN

                JSON_STRING_BEGIN "fixture" JSON_STRING_END JSON_NAME_SEPARATOR;

        WriteString(fixtureName);

        _stream << JSON_VALUE_SEPARATOR

                JSON_STRING_BEGIN "name" JSON_STRING_END JSON_NAME_SEPARATOR;

        WriteString(testName);

        _stream << JSON_VALUE_SEPARATOR;
        /*
                const std::vector<TestParameterDescriptor>& descs = parameters.m_params_desc();
                if (!descs.empty()) {
                        _stream << JSON_STRING_BEGIN "parameters" JSON_STRING_END JSON_NAME_SEPARATOR JSON_ARRAY_BEGIN;

                        for (std::size_t i = 0; i < descs.size(); ++i) {
                                if (i)
                                        _stream << JSON_VALUE_SEPARATOR;

                                const TestParameterDescriptor&    desc = descs[i];
                                _stream << JSON_OBJECT_BEGIN      JSON_STRING_BEGIN
                                    "declaration" JSON_STRING_END JSON_NAME_SEPARATOR;
                                WriteString(desc.Declaration);
                                _stream << JSON_VALUE_SEPARATOR JSON_STRING_BEGIN "value" JSON_STRING_END
           JSON_NAME_SEPARATOR; WriteString(desc.Value); _stream << JSON_OBJECT_END;
                        }

                        _stream << JSON_ARRAY_END JSON_VALUE_SEPARATOR;
                }
        */
        _stream << JSON_STRING_BEGIN "disabled" JSON_STRING_END JSON_NAME_SEPARATOR
                << (disabled ? JSON_TRUE : JSON_FALSE);
}

void JsonOutputter::EndTestObject() { _stream << JSON_OBJECT_END; }

void JsonOutputter::WriteString(const std::string& str)
{
        _stream << JSON_STRING_BEGIN;
        std::string::const_iterator it = str.begin();
        while (it != str.end()) {
                char c = *it++;
                switch (c) {
                case '\\':
                case '"':
                case '/': _stream << "\\" << c; break;
                case '\b': _stream << "\\b"; break;
                case '\f': _stream << "\\f"; break;
                case '\n': _stream << "\\n"; break;
                case '\r': _stream << "\\r"; break;
                case '\t': _stream << "\\t"; break;
                default: _stream << c; break;
                }
        }
        _stream << JSON_STRING_END;
}

void JsonOutputter::WriteDoubleProperty(const std::string& key, const double value)
{
        _stream << JSON_VALUE_SEPARATOR << JSON_STRING_BEGIN << key << JSON_STRING_END << JSON_NAME_SEPARATOR
                << std::fixed << std::setprecision(6) << (value / 1000000.0);
}

} // namespace myhayai
