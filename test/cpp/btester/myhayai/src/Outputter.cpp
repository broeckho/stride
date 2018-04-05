#include "myhayai/Outputter.hpp"

#include "myhayai/TestDescriptor.hpp"
#include "myhayai/TestResult.hpp"

#include <cstddef>
#include <iostream>

using namespace std;

namespace myhayai {

void Outputter::WriteTestNameToStream(ostream& stream, const string& fixtureName, const string& testName,
                                      const TestParametersDescriptor& parameters)
{
        stream << fixtureName << "." << testName;
        const vector<TestParameterDescriptor>& descs = parameters.Parameters();
        if (!descs.empty()) {
                stream << "(";
                for (size_t i = 0; i < descs.size(); ++i) {
                        if (i) {
                                stream << ", ";
                        }
                        const TestParameterDescriptor& desc = descs[i];
                        stream << desc.Declaration << " = " << desc.Value;
                }
                stream << ")";
        }
}

} // namespace myhayai
