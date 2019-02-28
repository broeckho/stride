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
#  Copyright 2018, Willem L, Kuylen E & Broeckhove J
#############################################################################

#================================================================================================
# Copy the rStride scripts to the bin folder.
#================================================================================================
# rStride main files
INSTALL(DIRECTORY
		   rstride
	   DESTINATION ${BIN_INSTALL_LOCATION}
)

# rStride analysis scripts
INSTALL( FILES 			
            rStride_all.R
            rStride_contacts.R
            rStride_explore.R
            rStride_immunity.R
            rStride_r0.R 						 
            rStride_reactive.R
	DESTINATION ${BIN_INSTALL_LOCATION}  
	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ
)

#############################################################################
