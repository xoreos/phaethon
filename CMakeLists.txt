cmake_minimum_required(VERSION 2.8.12)

project(phaethon C CXX)
set(phaethon_VERSION 0.0.2)


# -------------------------------------------------------------------------
# load some cmake modules from cmake/ subfolder
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(GNUInstallDirs)
include(CheckIncludeFile)
include(CMakeAM)


# -------------------------------------------------------------------------
# platform specific flags
if(CMAKE_HOST_APPLE)
  add_definitions(-DUNIX -DMACOSX)

elseif(CMAKE_HOST_UNIX)
  add_definitions(-DUNIX -DICONV_CONST=)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter -fdata-sections -ffunction-sections -fno-show-column -std=c++11")

  # Wrap libraries in --start-group and --end-group to easily support static linking and symbol resolution, maybe useful on APPLE also, but I don't know
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--gc-sections -Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--gc-sections -Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_C_CREATE_SHARED_MODULE "${CMAKE_C_CREATE_SHARED_MODULE}")
  string(REPLACE "<LINK_LIBRARIES>" "-Wl,--start-group <LINK_LIBRARIES> -Wl,--end-group" CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_MODULE}")
elseif(CMAKE_HOST_WIN32)
  add_definitions(-DICONV_CONST=)
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmt.lib")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  else()
    add_compile_options(-municode)
  endif()
else()
  message(STATUS "Unknown platform, maybe not supported")
endif()

# platform specific warning settings
if(CMAKE_HOST_WIN32)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
  if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(WARNINGS_DISABLE
      # ignore
      4250 # 'class1' : inherits 'class2::member' via dominance
      #4101 # 'identifier' : unreferenced local variable
      4100 # 'identifier' : unreferenced formal parameter
      4127 # conditional expression is constant (template parameter already evaluated)
      4189 # 'identifier' : local variable is initialized but not referenced
      4245 # 'conversion' : conversion from 'type1' to 'type2', signed/unsigned mismatch
      4435 # 'class1' : Object layout under /vd2 will change due to virtual base 'class2'
      4510 # 'class' : default constructor could not be generated
      4512 # 'class' : assignment operator could not be generated
      4610 # object 'class' can never be instantiated - user-defined constructor required
      #4702 # unreachable code
      4706 # assignment within conditional expression
      4710 # 'function' : function not inlined
      4714 # function 'function' marked as __forceinline not inlined

      # investigate later
      4305 # 'identifier' : truncation from 'type1' to 'type2'
      4244 # 'argument' : conversion from 'type1' to 'type2', possible loss of data
      4267 # 'var' : conversion from 'size_t' to 'type', possible loss of data
      4996 # 'function': was declared deprecated

      # investigate now
      #4005 # 'identifier' : macro redefinition
      #4800 # 'type' : forcing value to bool 'true' or 'false' (performance warning)
      #4146 # unary minus operator applied to unsigned type, result still unsigned
      )

    foreach(d ${WARNINGS_DISABLE})
      set(WARNINGS "${WARNINGS} /wd${d}")
    endforeach(d)

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARNINGS} ${MT_BUILD}")
  endif()
else()
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wno-multichar -Wpointer-arith -Wshadow")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-multichar -Wpointer-arith -Wshadow -Wnon-virtual-dtor")
endif()


# header detection, could get rid of it, but it is used by lua as well
include(CheckIncludeFile)
function(check_has_header FILE_NAME PP_NAME)
  check_include_file(${FILE_NAME} ${PP_NAME})
  if(${PP_NAME})
    add_definitions(-D${PP_NAME}=1)
  endif()
endfunction()

check_has_header("stdint.h"    HAVE_STDINT_H)
check_has_header("inttypes.h"  HAVE_INTTYPES_H)
check_has_header("sys/types.h" HAVE_SYS_TYPES_H)


# function detection
include(CheckCXXSymbolExists)
function(check_has_function FUNCTION_NAME FILE_NAME PP_NAME)
  check_cxx_symbol_exists(${FUNCTION_NAME} "${FILE_NAME}" ${PP_NAME})
  if(${PP_NAME})
    add_definitions(-D${PP_NAME}=1)
  endif()
endfunction()

check_has_function(strtof   "cstdlib" HAVE_STRTOF)
check_has_function(strtoll  "cstdlib" HAVE_STRTOLL)
check_has_function(strtoull "cstdlib" HAVE_STRTOULL)


# endianess detection, could be replaced by including Boost.Config
include(TestBigEndian)
test_big_endian(PHAETHON_BIG_ENDIAN)
if(PHAETHON_BIG_ENDIAN)
  add_definitions(-DPHAETHON_BIG_ENDIAN=1)
else()
  add_definitions(-DPHAETHON_LITTLE_ENDIAN=1)
endif()


# -------------------------------------------------------------------------
# subfolders where built binaries and libraries will be located, relative to the build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)


# -------------------------------------------------------------------------
# find the required libraries
set(PHAETHON_LIBRARIES "")

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${ZLIB_LIBRARIES})

option(Boost_USE_STATIC_LIBS ON)

find_package(Boost COMPONENTS system filesystem regex date_time atomic locale chrono thread REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${Boost_LIBRARIES})

find_package(OpenAL REQUIRED)
include_directories(${OPENAL_INCLUDE_DIR})
list(APPEND PHAETHON_LIBRARIES ${OPENAL_LIBRARY})

find_package(Mad REQUIRED)
include_directories(${MAD_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${MAD_LIBRARIES})

find_package(Faad REQUIRED)
include_directories(${FAAD_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${FAAD_LIBRARIES})

find_package(Vorbis REQUIRED)
include_directories(${VORBIS_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${VORBIS_LIBRARIES})

find_package(LibLZMA REQUIRED)
include_directories(${LIBLZMA_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${LIBLZMA_LIBRARIES})

find_package(wxWidgets COMPONENTS core base REQUIRED)
include_directories(${wxWidgets_INCLUDE_DIRS})
add_compile_options(${wxWidgets_CXX_FLAGS})
list(APPEND PHAETHON_LIBRARIES ${wxWidgets_LIBRARIES})

find_package(wxWidgets REQUIRED core base)
include(${wxWidgets_USE_FILE})
list(APPEND PHAETHON_LIBRARIES ${wxWidgets_LIBRARIES})

if(CMAKE_HOST_WIN32)
find_package(Ogg REQUIRED)
include_directories(${OGG_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${OGG_LIBRARIES})

find_package(Iconv REQUIRED)
include_directories(${ICONV_INCLUDE_DIRS})
list(APPEND PHAETHON_LIBRARIES ${ICONV_LIBRARIES})
endif()


# -------------------------------------------------------------------------
# phaethon main targets, parsed from configure.ac and */Makefile.am
include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
add_definitions(-DPACKAGE_STRING="phaethon ${phaethon_VERSION}")
parse_configure(configure.ac src)
target_link_libraries(phaethon ${PHAETHON_LIBRARIES})


# -------------------------------------------------------------------------
# try to add version information from git to src/common/version.cpp
# this is not 100% clean, and doesn't reconfigure when there's only a local change since last
#  build (so it does not always add the .dirty suffix)
string(TIMESTAMP PHAETHON_BUILDDATE "%Y-%m-%dT%H:%M:%SZ" UTC)
set_property(SOURCE src/common/version.cpp APPEND PROPERTY COMPILE_DEFINITIONS PHAETHON_BUILDDATE="${PHAETHON_BUILDDATE}")

find_package(Git)
if(GIT_FOUND)
  execute_process(COMMAND ${GIT_EXECUTABLE} -C ${CMAKE_CURRENT_SOURCE_DIR} describe --long --match desc/* OUTPUT_VARIABLE PHAETHON_REVDESC OUTPUT_STRIP_TRAILING_WHITESPACE)
  string(REGEX REPLACE "desc/(.*)-([^-]*)-([^-]*)" "\\1+\\2.\\3" PHAETHON_REVDESC "${PHAETHON_REVDESC}")

  execute_process(
    COMMAND ${GIT_EXECUTABLE} update-index --refresh --unmerged
    COMMAND ${GIT_EXECUTABLE} diff-index --quiet HEAD
    OUTPUT_QUIET ERROR_QUIET
    RESULT_VARIABLE PHAETHON_REVDIRT
  )
  if(NOT PHAETHON_REVDIRT EQUAL 0)
    set(PHAETHON_REVDESC "${PHAETHON_REVDESC}.dirty")
  endif()

  string(REGEX REPLACE "^[^+]*\\+" "" PHAETHON_REV "${PHAETHON_REVDESC}")
  message(STATUS "Building version: ${PHAETHON_REVDESC}")

  set_property(SOURCE src/common/version.cpp APPEND PROPERTY COMPILE_DEFINITIONS PHAETHON_REVDESC="${PHAETHON_REVDESC}")
  set_property(SOURCE src/common/version.cpp APPEND PROPERTY COMPILE_DEFINITIONS PHAETHON_REV="${PHAETHON_REV}")

  # use .git/logs/HEAD to track if git state has changed and if we need to reconfigure
  if(EXISTS ${CMAKE_SOURCE_DIR}/.git/logs/HEAD)
    configure_file(${CMAKE_SOURCE_DIR}/.git/logs/HEAD .dirstamp)
  endif()
endif()


# -------------------------------------------------------------------------
# documentation
option(PHAETHON_BUILD_DOCUMENTATION "Use Doxygen to create the HTML based API documentation" OFF)
if(PHAETHON_BUILD_DOCUMENTATION)
  find_package(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message(FATAL_ERROR "Doxygen is needed to build the documentation. Please install it correctly")
  endif()

  add_custom_target(doxygen
    COMMAND ${DOXYGEN_EXECUTABLE}
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    SOURCES ${PROJECT_SOURCE_DIR}/Doxyfile
  )
  add_custom_target(doxygen-clean
    COMMAND rm -rf doc/doxygen/html/
    COMMAND rm -rf doc/doxygen/latex/
    COMMAND rm -rf doc/doxygen/man/
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endif()