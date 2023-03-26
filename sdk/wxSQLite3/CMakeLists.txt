cmake_minimum_required(VERSION 3.9)

project(wxSQLite3)

set(WXSQLITE3_INCLUDES
	"include"
	"src")
set(WXSQLITE3_SRCS
	"src/sqlite3mc_amalgamation.h"
	"src/sqlite3mc_amalgamation.c"
	"src/sqlite3mc_config.h"
	"src/wxsqlite3.cpp")

find_package(wxWidgets COMPONENTS all REQUIRED)

if(wxWidgets_FOUND)
	include(${wxWidgets_USE_FILE})

	list(APPEND WXSQLITE3_INCLUDES ${wxWidgets_INCLUDE_DIRS})
	set(WXSQLITE3_LIBRARIES ${wxWidgets_LIBRARIES})

	add_library(wxSQLite3 SHARED ${WXSQLITE3_SRCS})

	target_include_directories(wxSQLite3 PUBLIC ${WXSQLITE3_INCLUDES})
	target_link_libraries(wxSQLite3 PRIVATE ${WXSQLITE3_LIBRARIES})
endif()
