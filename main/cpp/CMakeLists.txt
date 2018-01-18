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
    geo/GeoCoordinate.cpp
    geo/GeoCoordCalculator.cpp
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
    popgen/PopulationGenerator.cpp
    popgen/utils.cpp
    popgen/FamilyParser.cpp
#---
	sim/Simulator.cpp
	sim/SimulatorBuilder.cpp
	sim/StrideRunner.cpp
#---
	util/InstallDirs.cpp
	util/AliasDistribution.cpp
)

#============================================================================
# Build the Stride library.
#============================================================================
add_library(libstride ${LIB_SRC})
target_link_libraries(libstride ${LIBS})
target_compile_options(libstride PUBLIC "-fPIC")
set_target_properties(libstride PROPERTIES PREFIX "")

#============================================================================
# Build and install the stride executable.
#============================================================================
add_executable(stride sim/main.cpp)
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Build and install the stride executable.
#============================================================================
add_executable(popgen popgen/main.cpp)
target_link_libraries(popgen ${LIBS} libstride)
install(TARGETS popgen  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(MAIN_SRC)

#############################################################################
