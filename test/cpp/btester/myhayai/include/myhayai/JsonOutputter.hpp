#pragma once
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
 * Header file for JsonOutputter.
 */

#include "myhayai/Outputter.hpp"

#include <iomanip>
#include <ostream>

namespace myhayai {
/// JSON outputter.

/// Outputs the result of benchmarks in JSON format with the following
/// structure:
///
/// {
///     "format_version": 1,
///     "benchmarks": [{
///         "fixture": "DeliveryMan",
///         "name": "DeliverPackage",
///         "parameters": {
///             "declaration": "std::size_t distance",
///             "value": "1"
///         },
///         "disabled": false,
///         "runs": [{
///             "duration": 3801.889831
///         }, ..]
///     }, {
///         "fixture": "DeliveryMan",
///         "name": "DisabledTest",
///         "disabled": true
///     }, ..]
/// }
///
/// All durations are represented as milliseconds.
class JsonOutputter : public Outputter
{
public:
        /// Initialize JSON outputter.

        /// @param stream Output stream. Must exist for the entire duration of
        /// the outputter's use.
        explicit JsonOutputter(std::ostream& stream) : _stream(stream), _firstTest(true) {}

        void Begin(const std::size_t&, const std::size_t&) override;

        void End(const std::size_t&, const std::size_t&) override;

        void BeginTest(const std::string& fixtureName, const std::string& testName,
                       const TestParametersDescriptor& parameters, const std::size_t& runsCount) override;

        void SkipDisabledTest(const std::string& fixtureName, const std::string& testName,
                              const TestParametersDescriptor& parameters, const std::size_t& runsCount) override;

        void EndTest(const std::string& fixtureName, const std::string& testName,
                     const TestParametersDescriptor& parameters, const TestResult& result) override;

private:
        ///
        void BeginTestObject(const std::string& fixtureName, const std::string& testName,
                             const TestParametersDescriptor& parameters, const std::size_t& runsCount, bool disabled);

        ///
        void EndTestObject();

        /// Write an escaped string.
        /// The escaping is currently very rudimentary and assumes that names, parameters etc. are ASCII.
        /// @param str String to write.
        void WriteString(const std::string& str);

        /// Write a property with a double value.
        /// @param key Property key.
        /// @param value Property value.
        void WriteDoubleProperty(const std::string& key, const double value);

private:
        std::ostream& _stream;
        bool          _firstTest;
};

} // namespace myhayai
