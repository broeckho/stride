
#include "myhayai/TestParametersDescriptor.hpp"

#include "myhayai/Fixture.hpp"
#include "myhayai/TestFactory.hpp"

#include <cstring>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace myhayai {

vector<string> TestParametersDescriptor::ParseCommaSeparated(const char* separated)
{
        vector<string> result;

        if (*separated)
                ++separated;

        while ((*separated) && (*separated != ')')) {
                size_t  escapeCounter = 0;
                const char*  start         = separated;
                QuotingState state         = Unquoted;
                bool         escaped       = false;

                while (*separated) {
                        const char c = *separated++;
                        if (state == Unquoted) {
                                if ((c == '"') || (c == '\'')) {
                                        state   = (c == '"' ? DoubleQuoted : SingleQuoted);
                                        escaped = false;
                                } else if ((c == '<') || (c == '(') || (c == '[') || (c == '{'))
                                        ++escapeCounter;
                                else if ((escapeCounter) && ((c == '>') || (c == ')') || (c == ']') || (c == '}')))
                                        --escapeCounter;
                                else if ((!escapeCounter) && ((c == ',') || (c == ')'))) {
                                        result.push_back(TrimmedString(start, separated - 1));
                                        break;
                                }
                        } else {
                                if (escaped)
                                        escaped = false;
                                else if (c == '\\')
                                        escaped = true;
                                else if (c == (state == DoubleQuoted ? '"' : '\''))
                                        state = Unquoted;
                        }
                }
        }

        return result;
}

TestParameterDescriptor TestParametersDescriptor::ParseDescriptor(const string& raw)
{
        const char* position = raw.c_str();

        // Split the declaration into its declaration and its default type.
        const char*  equalPosition = nullptr;
        size_t  escapeCounter = 0;
        QuotingState state         = Unquoted;
        bool         escaped       = false;

        while (*position) {
                const char c = *position++;

                if (state == Unquoted) {
                        if ((c == '"') || (c == '\'')) {
                                state   = (c == '"' ? DoubleQuoted : SingleQuoted);
                                escaped = false;
                        } else if ((c == '<') || (c == '(') || (c == '[') || (c == '{'))
                                ++escapeCounter;
                        else if ((escapeCounter) && ((c == '>') || (c == ')') || (c == ']') || (c == '}')))
                                --escapeCounter;
                        else if ((!escapeCounter) && (c == '=')) {
                                equalPosition = position;
                                break;
                        }
                } else {
                        if (escaped)
                                escaped = false;
                        else if (c == '\\')
                                escaped = true;
                        else if (c == (state == DoubleQuoted ? '"' : '\''))
                                state = Unquoted;
                }
        }

        // Construct the parameter descriptor.
        if (equalPosition) {
                const char* start = raw.c_str();
                const char* end   = start + raw.length();

                return TestParameterDescriptor(string(TrimmedString(start, equalPosition - 1)),
                                               string(TrimmedString(equalPosition, end)));
        } else
                return TestParameterDescriptor(raw, string());
}

TestParametersDescriptor::TestParametersDescriptor(const char* rawDeclarations, const char* rawValues)
{
        // Parse the declarations.
        vector<string> declarations = ParseCommaSeparated(rawDeclarations);
        for (const auto& d : declarations) {
                _parameters.push_back(ParseDescriptor(d));
        }

        // Parse the values.
        vector<string> values = ParseCommaSeparated(rawValues);
        size_t straightValues      = (_parameters.size() > values.size() ? values.size() : _parameters.size()),
                    variadicValues      = 0;

        if (values.size() > _parameters.size()) {
                if (straightValues > 0)
                        --straightValues;
                variadicValues = values.size() - _parameters.size() + 1;
        }
        for (size_t i = 0; i < straightValues; ++i) {
                _parameters[i].Value = values[i];
        }
        if (variadicValues) {
                stringstream variadic;
                for (size_t i = 0; i < variadicValues; ++i) {
                        if (i) {
                                variadic << ", ";
                        }
                        variadic << values[straightValues + i];
                }
                _parameters[_parameters.size() - 1].Value = variadic.str();
        }
}

} // namespace myhayai
