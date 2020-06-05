cmake_minimum_required(VERSION 3.8)

#TO-DO 
#Select either multi-header or single-header install option
#Generate and install cmake config files for find_package()
#Generate and install doxygen docs

project(aviana_signal VERSION 0.9.0 LANGUAGES CXX)

option(ENABLE_TESTING "Build tests" ON)
option(ENABLE_INSTALL "Install headers" OFF)

include(GNUInstallDirs)

set(SIGNAL_TARGET_NAME 			${PROJECT_NAME})
set(SIGNAL_INCLUDE_INSTALL_DIR 	"${CMAKE_INSTALL_INCLUDEDIR}/aviana/")
set(SIGNAL_INCLUDE_BUILD_DIR	"${PROJECT_SOURCE_DIR}/include/")

add_library(${SIGNAL_TARGET_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${SIGNAL_TARGET_NAME} ALIAS ${SIGNAL_TARGET_NAME})
target_compile_features(${SIGNAL_TARGET_NAME} INTERFACE cxx_std_20)
target_include_directories(
	${SIGNAL_TARGET_NAME} 
	INTERFACE
	$<BUILD_INTERFACE:  ${SIGNAL_INCLUDE_BUILD_DIR}  >
	$<INSTALL_INTERFACE:${SIGNAL_INCLUDE_INSTALL_DIR}>
)


if(ENABLE_TESTING)
	enable_testing()
	#add_subdirectory(test)
endif()

if(ENABLE_INSTALL)
	install(
		DIRECTORY	${SIGNAL_INCLUDE_BUILD_DIR}
		DESTINATION ${SIGNAL_INCLUDE_INSTALL_DIR}
	)
endif()