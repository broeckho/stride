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
    Benchmark.hpp
    Console.hpp
    Test.hpp
    InfoFactory.hpp
    Payload.hpp
    Subject.hpp
    Test.hpp
    TestDescriptor.hpp
    TestFactory.hpp
    TestResult.hpp
#
    BenchmarkRunner.cpp
    BoxPlotData.cpp
    ConsoleViewer.cpp
    Id.cpp
    CliController.cpp
    PtreeViewer.cpp
    TestDescriptors.cpp
)

add_library(myhayai STATIC ${SRC})
set_property(TARGET myhayai PROPERTY POSITION_INDEPENDENT_CODE ON)

#============================================================================
# Clean up.
#============================================================================
unset(SRC)

#############################################################################
