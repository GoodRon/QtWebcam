cmake_minimum_required(VERSION 3.0)

if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "../install")
endif()

if(NOT DEFINED CMAKE_PREFIX_PATH)
	if(MSVC)
		set(CMAKE_PREFIX_PATH "F:\\soft\\Qt\\5.5\\msvc2013")
	endif()
endif()

if(MSVC)
	add_definitions(-D_WIN32_WINNT=0x0501)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
elseif(CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pipe -std=c++11 ")
endif()

if (NOT DEFINED BIN_DIR)
	set(BIN_DIR bin)
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_definitions(${Qt5Widgets_DEFINITIONS})
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
if( MSVC )
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:console")
endif()

set(QT_LIBS Qt5::Core Qt5::Widgets Qt5::Gui)

set(SRC main.cpp
		WebcamWindow.cpp
                VideoDevice.cpp
                VideoCapture.cpp
	)

set(LIBS ${QT_LIBS}
                strmiids Ole32 OleAut32
	)

add_executable(webcam ${SRC})
target_link_libraries(webcam ${LIBS})
install(TARGETS webcam RUNTIME DESTINATION ${BIN_DIR}/)

foreach(lib ${QT_LIBS})
    get_target_property(DLL ${lib} "LOCATION")
    install(FILES ${DLL} DESTINATION bin)
endforeach()
get_target_property(DLL Qt5::QWindowsIntegrationPlugin "LOCATION")
install(FILES ${DLL} DESTINATION bin/platforms)
