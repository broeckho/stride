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
#  Copyright 2019, Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# UNZIP files from data/* to ${DATA_INSTALL_LOCATION}
#============================================================================
# Create build folder to unzip all compressed files
set(UNZIP_FOLDER ${CMAKE_BINARY_DIR}/main/resources/data_zip/)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${UNZIP_FOLDER})

# Unzip all ZIP files
file(GLOB ZIP_FILES "data/*.zip")
foreach( file_i ${ZIP_FILES} )
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${file_i}
                    WORKING_DIRECTORY ${UNZIP_FOLDER})
endforeach()

# Install unzipped files
file(GLOB DEPLOY_FILES ${UNZIP_FOLDER}*)
INSTALL(FILES ${DEPLOY_FILES} DESTINATION ${DATA_INSTALL_LOCATION})
unset(UNZIP_FOLDER)

#============================================================================
# Copy all non-zipped files from data/* to ${DATA_INSTALL_LOCATION}
#============================================================================
file(GLOB DEPLOY_FILES "data/*")
list(REMOVE_ITEM DEPLOY_FILES ${ZIP_FILES})
INSTALL(FILES ${DEPLOY_FILES} DESTINATION ${DATA_INSTALL_LOCATION})
unset(ZIP_FILES)
unset(DEPLOY_FILES)

#============================================================================
# Copy all files from config/* to ${CONFIG_INSTALL_LOCATION}
#============================================================================
file(GLOB DEPLOY_FILES "config/*")
INSTALL(FILES ${DEPLOY_FILES} DESTINATION ${CONFIG_INSTALL_LOCATION})
unset(DEPLOY_FILES)

#============================================================================
# Add subdirs:
#============================================================================
add_subdirectory(lib)

#############################################################################