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
#  Copyright 2017, Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# Sources.
#============================================================================
set(LIB_SRC
#---
    calendar/Calendar.cpp
#---
    core/Cluster.cpp
    core/ClusterType.cpp
    core/ContactProfile.cpp
    core/DiseaseProfile.cpp
    core/Health.cpp
    core/Infector.cpp
    core/LogMode.cpp
#---
	immunity/Vaccinator.cpp
#---
	output/AdoptedFile.cpp
	output/CasesFile.cpp
	output/PersonFile.cpp
	output/SummaryFile.cpp
#---	
    pop/Person.cpp
    pop/PopulationBuilder.cpp
#---
	sim/Simulator.cpp
	sim/SimulatorBuilder.cpp
	sim/StrideRunner.cpp
#---
	util/InstallDirs.cpp
)

set(MAIN_SRC
	sim/main.cpp
)

#============================================================================
# Build the Stride library.
#============================================================================
add_library(libstride ${LIB_SRC})
target_link_libraries(libstride ${LIBS})
target_compile_options(libstride PUBLIC "-fPIC")
set_target_properties(libstride PROPERTIES PREFIX "")

#============================================================================
# Build & install the Swig files.
#============================================================================
set(SWIG_IFACE
	swig/health.i
	swig/population.i
	swig/simulator.i
	swig/simulatorobserver.i
	swig/striderunner.i
	swig/stride.i
)

find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
find_package(PythonLibs 3 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
set_property(SOURCE ${SWIG_IFACE} PROPERTY CPLUSPLUS ON)
swig_add_library(stride LANGUAGE python SOURCES swig/stride.i TYPE MODULE)
swig_link_libraries(stride PUBLIC libstride ${PYTHON_LIBRARIES})

#============================================================================
# Build and install the stride executable.
#============================================================================
add_executable(stride ${MAIN_SRC})
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(LIB_SRC)
unset(MAIN_SRC)
unset(SWIG_IFACE)

#############################################################################
