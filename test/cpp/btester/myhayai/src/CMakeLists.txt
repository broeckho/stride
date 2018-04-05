#############################################################################
#  This file is part of the Stride software.
#  It is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or any
#  later version.
#  The software is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  You should have received a copy of the GNU General Public License,
#  along with the software. If not, see <http://www.gnu.org/licenses/>.
#  see http://www.gnu.org/licenses/.
#
#  Copyright 2018 Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# Build myhayai lib (build only, no install)
#============================================================================
set(SRC
#
    ../include/myhayai/Benchmarker.hpp
    ../include/myhayai/Clock.hpp
    ../include/myhayai/Console.hpp
    ../include/myhayai/ConsoleFileOutputter.hpp
    ../include/myhayai/ConsoleOutputter.hpp
    ../include/myhayai/Fixture.hpp
    ../include/myhayai/JsonOutputter.hpp
    ../include/myhayai/JsonFileOutputter.hpp
    ../include/myhayai/JUnitXmlFileOutputter.hpp
    ../include/myhayai/JunitXmlOutputter.hpp
    ../include/myhayai/Outputter.hpp
    ../include/myhayai/TestFactory.hpp
    ../include/myhayai/TestFactoryDefault.hpp
    ../include/myhayai/TestDescriptor.hpp
    ../include/myhayai/TestParameterDescriptor.hpp
    ../include/myhayai/TestParameterDescriptor.hpp
    ../include/myhayai/TestParametersDescriptor.hpp
    ../include/myhayai/TestResult.hpp
    ../include/myhayai/Fixture.hpp
    ../include/myhayai/TestFactory.hpp
    ../include/myhayai/TestDescriptor.hpp
    ../include/myhayai/MainRunner.hpp
#
    Benchmarker.cpp
    ConsoleOutputter.cpp
    JsonOutputter.cpp
    JUnitXmlOutputter.cpp
    Outputter.cpp
    MainRunner.cpp
    TestParametersDescriptor.cpp
    TestResult.cpp
)

add_library(myhayai STATIC ${SRC})
target_compile_options(myhayai PUBLIC -I ${CMAKE_HOME_DIRECTORY}/test/cpp/btester/myhayai/include)
set_property(TARGET myhayai PROPERTY POSITION_INDEPENDENT_CODE ON)

#============================================================================
# Clean up.
#============================================================================
unset(SRC)

#############################################################################