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
#  Copyright 2017, 2018 Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# Sources. Includes headers to enable code insight by CLion.
#============================================================================
set(LIB_SRC
#---
	behaviour/BeliefSeeder.cpp
	behaviour/behaviour_policies/NoBehaviour.h
	behaviour/behaviour_policies/Vaccination.h
	behaviour/belief_policies/Belief.h
	behaviour/belief_policies/HBM.h
	behaviour/belief_policies/Imitation.h
	behaviour/belief_policies/NoBelief.h
	behaviour/belief_policies/Threshold.h
	behaviour/information_policies/LocalDiscussion.h
	behaviour/information_policies/NoGlobalInformation.h
	behaviour/information_policies/NoLocalInformation.h
#---
    calendar/Calendar.cpp
    calendar/DaysOffAll.h
    calendar/DaysOffInterface.h
    calendar/DaysOffNone.h
    calendar/DaysOffSchool.h
    calendar/DaysOffStandard.h
#---
	contact/AgeContactProfile.cpp
	contact/AgeContactProfiles.h
	contact/ContactHandler.h
	contact/ContactLogMode.cpp
	contact/Infector.cpp
	contact/InfectorExec.h
	contact/InfectorMap.h
    contact/TransmissionProfile.cpp
#---
    disease/DiseaseSeeder.cpp
	disease/Health.cpp
    disease/HealthSeeder.cpp
	disease/Immunizer.cpp
#---
	output/AdoptedFile.cpp
    output/InfectedFile.cpp
    output/PersonsFile.cpp
	output/SummaryFile.cpp
#---
	pool/ContactPool.cpp
	pool/ContactPoolSys.h
	pool/ContactPoolType.cpp
    pool/IdSubscriptArray.h
#---
    pop/Age.h
    pop/Person.cpp
    pop/Population.cpp
	pop/PopBuilder.cpp
    pop/SurveySeeder.cpp
#---
    sim/CliController.cpp
    sim/ControlHelper.cpp
	sim/SimRunner.cpp
    sim/Sim.cpp
    sim/SimBuilder.cpp
	sim/StanController.cpp
    sim/event/Id.cpp
	sim/event/Subject.cpp
#---
    util/Any.h
	util/BoxplotData.h
    util/ConfigInfo.cpp
    util/CSV.cpp
	util/CSVRow.cpp
    util/FileSys.cpp
	util/GnuPlotCSV.cpp
    util/GnuPlot.cpp
	util/LogUtils.cpp
	util/PtreeUtils.h
    util/RNEngineType.cpp
    util/RNManager.cpp
    util/RunConfigManager.cpp
    util/SegmentedVector.h
    util/SliceIndexer.h
    util/Stopwatch.h
    util/StringUtils.h
    util/Subject.h
    util/SVIterator.h
    util/TimeStamp.h
    util/TimeToString.h
#---
    viewers/AdoptedFileViewer.cpp
    viewers/InfectedFileViewer.cpp
    viewers/InfectedViewer.cpp
    viewers/CliViewer.cpp
    viewers/PersonsFileViewer.cpp
    viewers/SummaryFileViewer.cpp
)

#============================================================================
# Build the Stride library.
#============================================================================
add_library(libstride ${LIB_SRC})
set_source_files_properties(BeliefSeeder.cpp PROPERTIES COMPILE_FLAGS "-Wno-unused-variable")
target_link_libraries(libstride ${LIBS})
target_compile_options(libstride PUBLIC "-fPIC")
target_compile_definitions(libstride PUBLIC "-DSTRIDE_GIT_HASH=${STRIDE_WC_REVISION_HASH}")
set_target_properties(libstride PROPERTIES PREFIX "")

#============================================================================
# Build and install the stride executable.
#============================================================================
add_executable(stride execs/main.cpp)
target_link_libraries(stride ${LIBS} libstride)
install(TARGETS stride  DESTINATION   ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(LIB_SRC)

#############################################################################
