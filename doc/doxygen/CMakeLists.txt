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
#  Copyright 2016, 2018, Willem L, Kuylen E & Broeckhove J
#############################################################################

#============================================================================
# api_doc
#============================================================================
find_package(Doxygen)

if( DOXYGEN_FOUND )	

	#=== dot tool ===
	if(DOXYGEN_DOT_FOUND)
		set(DOXY_HAVE_DOT YES)
	else()
		set(DOXY_HAVE_DOT NO)
	endif()
	
	#=== setup ===			
	set(DOXY_HTML_DIR 	"html")
	configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.cmake.in 
					${CMAKE_CURRENT_BINARY_DIR}/Doxyfile )	

	#=== target ===
	add_custom_target(api_doc ALL	${DOXYGEN_EXECUTABLE} Doxyfile)
			 
	#=== install ===
	install(DIRECTORY		${CMAKE_CURRENT_BINARY_DIR}/${DOXY_HTML_DIR}
	       DESTINATION		${DOC_INSTALL_LOCATION} )
	install(FILES      		ReferenceManual.html
	       DESTINATION		${DOC_INSTALL_LOCATION} )
		
	#=== unset ===
	unset( DOXY_HAVE_DOT )
	unset( DOXY_HTML_DIR )
	
endif( DOXYGEN_FOUND )

#############################################################################