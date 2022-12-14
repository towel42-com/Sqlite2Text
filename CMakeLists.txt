SET(CMAKE_CONFIGURATION_TYPES Debug RelWithDebInfo CACHE TYPE INTERNAL FORCE) 
cmake_minimum_required(VERSION 3.0)
project(SQLite2Txt)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/Modules ${CMAKE_MODULE_PATH})
find_package(BPSQt55 REQUIRED)
find_package(CreateUserVCProj REQUIRED)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0043 NEW)


IF(CMAKE_SIZEOF_VOID_P EQUAL 4) #32 bit
	SET( BITSIZE 32 )
ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 8) #64 bit
	SET( BITSIZE 64 )
ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8) 
	MESSAGE( STATUS "Unknown Bitsize - CMAKE_SIZEOF_VOID_P not set to 4 or 8" )
	MESSAGE( STATUS "-DCMAKE_SIZEOF_VOID_P=4 for 32 bit" )
	MESSAGE( FATAL_ERROR "-DCMAKE_SIZEOF_VOID_P=8 for 64 bit" )
ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 4) 

Message( STATUS "Building application with pointer size ${BITSIZE} bits" )

#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation
#

include(include.cmake)
include(Project.cmake )
set(QT_LIBRARIES 
	Qt5::Core
	Qt5::Sql
	)
QT5_ADD_RESOURCES( qtproject_QRC_SRCS ${qtproject_QRC} )

add_executable(SQLite2Txt 
	${qtproject_SRCS} 
	${qtproject_UIS_H} 
	${qtproject_H} 
	${qtproject_MOC_SRCS} 
	${qtproject_QRC_SRCS} 
	) 
set_target_properties( SQLite2Txt PROPERTIES FOLDER Applications )
SET(CMAKE_INSTALL_PREFIX "E:/Dropbox/SQLite2Txt" )

CreateUserProj()
    SET(QT_DONT_USE_QTCORE FALSE)
    SET(QT_DONT_USE_QTGUI TRUE)
    SET(QT_USE_QTSQL TRUE)
    BPS_Install_Qt()
IF(WIN32)
	SET( EXTRA_LIBS Iphlpapi.lib psapi.lib )
ELSE(WIN32)
	SET( EXTRA_LIBS "" )
ENDIF(WIN32)

IF(WIN32)
	SET_TARGET_PROPERTIES(SQLite2Txt PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcprt.lib"
		)
ENDIF(WIN32)

target_link_libraries( SQLite2Txt 
	${QT_QTMAIN_LIBRARY} 
	${QT_LIBRARIES} 
)

INSTALL(TARGETS SQLite2Txt RUNTIME DESTINATION . )

IF(WIN32)
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/Debug/SQLite2Txt.pdb DESTINATION . CONFIGURATIONS Debug )
ENDIF()
