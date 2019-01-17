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
# Subdirectories.
#============================================================================
add_subdirectory(gengeopop)

#============================================================================
# Sources.
#============================================================================
set(LIB_SRC
#---
    calendar/Calendar.cpp
#---
	contact/AgeContactProfile.cpp
	contact/ContactLogMode.cpp
	contact/Infector.cpp
    contact/TransmissionProfile.cpp
#---
    disease/DiseaseSeeder.cpp
	disease/Health.cpp
    disease/HealthSeeder.cpp
    disease/PublicHealthAgency.cpp
#---
	output/AdoptedFile.cpp
    output/InfectedFile.cpp
    output/PersonsFile.cpp
	output/SummaryFile.cpp
#---
	pool/ContactPool.cpp
	pool/ContactPoolType.cpp
#---
    pop/Person.cpp
    pop/Population.cpp
    pop/DefaultPopBuilder.cpp
    pop/GenPopBuilder.cpp
    pop/ImportPopBuilder.cpp
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
	util/CSVRow.cpp
	util/LogUtils.cpp
	util/Rn.cpp
    util/CSV.cpp
    util/ConfigInfo.cpp
    util/FileSys.cpp
    util/RunConfigManager.cpp
#---
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
target_link_libraries(libstride ${LIBS} libgengeopop)
target_compile_options(libstride PUBLIC "-fPIC")
target_compile_definitions(libstride PUBLIC "-DSTRIDE_GIT_HASH=${STRIDE_WC_REVISION_HASH}")
set_target_properties(libstride PROPERTIES PREFIX "")

#============================================================================
# Build the stride binary.
#============================================================================
add_executable(stride execs/stride.cpp)
target_link_libraries(stride ${LIBS} libstride libgengeopop)
install(TARGETS stride DESTINATION ${BIN_INSTALL_LOCATION})

#============================================================================
# Build the genpop binary.
#============================================================================
add_executable(genpop execs/genpop.cpp)
target_link_libraries(genpop ${LIBS} libprotobuf_insrc libproto libstride libgengeopop)
install(TARGETS genpop DESTINATION ${BIN_INSTALL_LOCATION})

#============================================================================
# Clean up.
#============================================================================
unset(LIB_SRC)

#############################################################################
