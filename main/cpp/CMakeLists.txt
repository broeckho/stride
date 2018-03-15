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
	core/ContactPool.cpp
    core/ContactPoolType.cpp
    core/ContactProfile.cpp
    core/DiseaseProfile.cpp
    core/Health.cpp
    core/Infector.cpp
    core/LogMode.cpp
#---
	immunity/Immunizer.cpp
	immunity/Vaccinator.cpp
#---
	output/AdoptedFile.cpp
	output/CasesFile.cpp
    output/PersonsFile.cpp
	output/SummaryFile.cpp
#---	
    pop/Person.cpp
    pop/Population.cpp
    pop/PopulationBuilder.cpp
#---
    sim/CliController.cpp
	sim/SimRunner.cpp
	sim/Simulator.cpp
	sim/SimulatorBuilder.cpp
    sim/event/Id.cpp
	sim/event/Subject.cpp
	sim/python/StrideRunner.cpp
#---
    util/CSV.cpp
	util/CSVRow.cpp
    util/FileUtils.cpp
    util/FileSys.cpp
	util/LogUtils.cpp
	util/PtreeUtils.cpp
    util/RNEngineType.cpp
    util/RNManager.cpp
#---
	viewers/AdoptedViewer.cpp
	viewers/CasesViewer.cpp
    viewers/CliViewer.cpp
    viewers/PersonsViewer.cpp
	viewers/SummaryViewer.cpp
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
target_include_directories(stride PUBLIC SYSTEM
	${CMAKE_HOME_DIRECTORY}/main/resources/lib/tclap/include)
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(MAIN_SRC)

#############################################################################
