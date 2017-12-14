#################################################################################################
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
#  Copyright 2016, Willem L, Kuylen E & Broeckhove J
#################################################################################################

#================================================================================================
# List python example scripts.
#================================================================================================

SET(PYTHON_EXAMPLES
	runCallbackInterval.py
	runCallbackTimestep.py
	runExperiments.py
	runManualConfig.py
	runRepeat.py
	runSimple.py
	runStopOnCallback.py
)

#================================================================================================
# Copy Python scripts to the bin folder.
#================================================================================================


file(GLOB DEPLOY_FILES "data/*")
INSTALL( FILES ${DEPLOY_FILES} DESTINATION ${BIN_INSTALL_LOCATION}/data )

file(GLOB DEPLOY_FILES "config/*")
INSTALL( FILES ${DEPLOY_FILES} DESTINATION ${BIN_INSTALL_LOCATION}/config )

INSTALL ( FILES
		  ${PYTHON_EXAMPLES}
		  DESTINATION ${BIN_INSTALL_LOCATION}
		  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_WRITE GROUP_READ
)

#================================================================================================
# Clean up.
# ===============================================================================================

unset(PYTHON_EXAMPLES)

#################################################################################################