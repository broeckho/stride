#############################################################################
# CMake find_package module
# Adapted by Jan Broeckhove, CoMP/UA, 2011
#############################################################################
#
# FIND_PACKAGE_HANDLE_STANDARD_ARGS(NAME (DEFAULT_MSG|"Custom failure message") VAR1 ... )
#
#    This macro is intended to be used in FindXXX.cmake modules files.
#    It handles the REQUIRED and QUIET argument to FIND_PACKAGE() and
#    it also sets the <UPPERCASED_NAME>_FOUND variable.
#    The package is found if all variables listed are TRUE.
#    Example:
#
#    FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibXml2 DEFAULT_MSG LIBXML2_LIBRARIES LIBXML2_INCLUDE_DIR)
#
#    LibXml2 is considered to be found, if both LIBXML2_LIBRARIES and 
#    LIBXML2_INCLUDE_DIR are valid. Then also LIBXML2_FOUND is set to TRUE.
#    If it is not found and REQUIRED was used, it fails with FATAL_ERROR, 
#    independent whether QUIET was used or not.
#    If it is found, the location is reported using each macro argument, so 
#    here a message "Found LibXml2: /usr/lib/libxml2.so" will be printed out.
#    If the second argument is DEFAULT_MSG, the message in the failure case will 
#    be "Could NOT find LibXml2", if you don't like this message you can specify
#    your own custom failure message there.
#
#############################################################################

MACRO(FIND_PACKAGE_HANDLE_STANDARD_ARGS _NAME _FAIL_MSG _VAR1 )
  	
#############################################################################
# Adapt the fail message
#############################################################################
  	if ( "${_FAIL_MSG}" STREQUAL "DEFAULT_MSG" )
    	if ( ${_NAME}_FIND_REQUIRED )
      		set( _FAIL_MESSAGE "Could not find REQUIRED package ${_NAME}" )
    	else ( ${_NAME}_FIND_REQUIRED )
      		set( _FAIL_MESSAGE "Could not find OPTIONAL package ${_NAME}" )
    	endif ( ${_NAME}_FIND_REQUIRED )
  	else ( "${_FAIL_MSG}" STREQUAL "DEFAULT_MSG" )
    	set( _FAIL_MESSAGE "${_FAIL_MSG}" )
  	endif ( "${_FAIL_MSG}" STREQUAL "DEFAULT_MSG" )
  	
#############################################################################
# Logic for the FOUND variable
#############################################################################
  	string( TOUPPER ${_NAME} _NAME_UPPER )
  	
  	set( ${_NAME_UPPER}_FOUND TRUE )
  	if( NOT ${_VAR1} )
    	set( ${_NAME_UPPER}_FOUND FALSE )
  	endif( NOT ${_VAR1} )

  	foreach( _CURRENT_VAR ${ARGN} )
    	if( NOT ${_CURRENT_VAR} )
      		set( ${_NAME_UPPER}_FOUND FALSE )
    	endif( NOT ${_CURRENT_VAR} )
  	endforeach( _CURRENT_VAR )

#############################################################################
# Report what was found
#############################################################################
  	if ( ${_NAME_UPPER}_FOUND )
    	if ( NOT ${_NAME}_FIND_QUIETLY )
        	message( STATUS "Found ${_NAME}: ${${_VAR1}}" )
   			foreach( _CURRENT_VAR ${ARGN} )
        		message( STATUS "Found ${_NAME}: ${${_CURRENT_VAR}}" )
   			endforeach( _CURRENT_VAR )         
    	endif ( NOT ${_NAME}_FIND_QUIETLY )
  	else ( ${_NAME_UPPER}_FOUND )
    	if ( ${_NAME}_FIND_REQUIRED )
        	message( FATAL_ERROR "${_FAIL_MESSAGE}" )
    	else ( ${_NAME}_FIND_REQUIRED )
      		if ( NOT ${_NAME}_FIND_QUIETLY )
        		message( STATUS "${_FAIL_MESSAGE}" )
      		endif ( NOT ${_NAME}_FIND_QUIETLY )
    	endif ( ${_NAME}_FIND_REQUIRED )
  	endif ( ${_NAME_UPPER}_FOUND )
  	
ENDMACRO(FIND_PACKAGE_HANDLE_STANDARD_ARGS)

#############################################################################
