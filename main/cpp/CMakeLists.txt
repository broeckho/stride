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
# Sources. Includes headers to enable code insight by CLion.
#============================================================================
set(LIB_SRC
#---
    calendar/Calendar.cpp
#---
	contact/AgeContactProfile.cpp
	contact/AgeContactProfiles.h
	contact/ContactHandler.h
	contact/ContactLogMode.cpp
	contact/Infector.cpp
    contact/TransmissionProfile.cpp
#---
    disease/DiseaseSeeder.cpp
	disease/Health.cpp
    disease/HealthSeeder.cpp
	disease/Immunizer.cpp
#---
	output/AdoptedFile.cpp
	output/CasesFile.cpp
    output/PersonsFile.cpp
	output/SummaryFile.cpp
#---
	pool/ContactPool.cpp
	pool/ContactPoolSys.h
	pool/ContactPoolType.cpp
    pool/IdSubscriptArray.h
#---
    pop/Age.h
    pop/ContactPoolBuilder.cpp
    pop/Person.cpp
    pop/Population.cpp
    pop/PopulationBuilder.cpp
    pop/SurveySeeder.cpp
#---
    sim/CliController.cpp
	sim/SimRunner.cpp
	sim/Simulator.cpp
	sim/SimulatorBuilder.cpp
    sim/event/Id.cpp
    sim/event/Payload.h
	sim/event/Subject.cpp
	sim/python/StrideRunner.cpp
#---
    util/ConfigInfo.h
    util/CSV.cpp
	util/CSVRow.cpp
    util/FileSys.cpp
	util/LogUtils.cpp
    util/RNEngineType.cpp
    util/RNManager.cpp
	util/RunConfigPtree.cpp
    util/RunConfigString.cpp
    util/SegmentedVector.h
    util/Stopwatch.h
    util/StringUtils.h
    util/Subject.h
    util/SVIterator.h
    util/TimeStamp.h
    util/TimeToString.h
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
include_directories(SYSTEM ${CMAKE_HOME_DIRECTORY}/main/resources/lib/tclap/include)
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(MAIN_SRC)

#############################################################################
