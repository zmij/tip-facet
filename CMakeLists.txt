#	CMakeLists.txt for tip-facet library
#	
#	@author zmij
#	@date Nov 30, 2015

cmake_minimum_required(VERSION 2.6)

set(lib_name facet)
string(TOUPPER ${lib_name} LIB_NAME)

install(
    FILES include/tip/util/facet.hpp
    DESTINATION include/tip/util
)

if(BUILD_TESTS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
    enable_testing()
    add_subdirectory(test)
endif()

get_directory_property(has_parent PARENT_DIRECTORY)
if (has_parent)
    set(TIP_${LIB_NAME}_LIB ${PROJECT_PREFIX}-${lib_name} CACHE INTERNAL "Name of tip-facet library target")
    set(TIP_${LIB_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "Path to tip-facet libaray includes" )
endif()
