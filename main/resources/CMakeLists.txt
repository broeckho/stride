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
# Copy all files from data/* to ${DATA_INSTALL_LOCATION}
#============================================================================
file(GLOB DEPLOY_FILES "data/*")
INSTALL(FILES ${DEPLOY_FILES} DESTINATION ${DATA_INSTALL_LOCATION} )

#============================================================================
# UNZIP files from data/* to ${DATA_INSTALL_LOCATION}
#============================================================================
file(GLOB DEPLOY_FILES "data/*.zip")
add_custom_target( unZip ALL)
add_custom_command(TARGET unZip PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar xzf ${DEPLOY_FILES}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	DEPENDS ${DEPLOY_FILES}
	VERBATIM
	)

file(GLOB DEPLOY_FILES ${CMAKE_BINARY_DIR}/*.csv)
INSTALL(FILES ${DEPLOY_FILES} DESTINATION ${DATA_INSTALL_LOCATION} )
	
#============================================================================
# Copy all files from config/* to ${CONFIG_INSTALL_LOCATION}
#============================================================================
file(GLOB DEPLOY_FILES "config/*")
INSTALL( FILES ${DEPLOY_FILES} DESTINATION ${CONFIG_INSTALL_LOCATION} )
unset(DEPLOY_FILES)

#============================================================================
# Add subdirs:
#============================================================================
add_subdirectory( lib )

#############################################################################