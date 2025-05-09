CMAKE_MINIMUM_REQUIRED(VERSION 3.21)

PROJECT("Scan Tailor Deviant" LANGUAGES CXX)

IF (POLICY CMP0072)
        CMAKE_POLICY(SET CMP0072 NEW)
ENDIF()

SET(
        CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
        "${CMAKE_SOURCE_DIR}/cmake/default_cxxflags.cmake"
)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

IF(MSVC)
        # Disable checked iterators for extra performance.
        ADD_DEFINITIONS(-D_SECURE_SCL=0)
ENDIF()

IF(DEBUG_CLI)
        ADD_DEFINITIONS(-DDEBUG_CLI)
ENDIF(DEBUG_CLI)

OPTION(ENABLE_TESTS "Enable testing. For debug purpose only. Do not enable if you are not a programmer." OFF)
IF(ENABLE_TESTS)
        ENABLE_TESTING()
ENDIF()

# An undocumented side-effect of CONFIGURE_FILE() is that it makes
# the whole project depend on the file we are parsing / copying.
CONFIGURE_FILE(
        "${PROJECT_SOURCE_DIR}/version.h"
        "${PROJECT_BINARY_DIR}/.version.h" COPYONLY
)

# Prevent this leftover from old builds to be used in favour
# of the one in ${PROJECT_SOURCE_DIR}
IF(NOT "${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}")
        FILE(REMOVE "${PROJECT_BINARY_DIR}/version.h")
ENDIF()

# Extract VERSION and VERSION_QUAD from version.h
FILE(READ "${PROJECT_SOURCE_DIR}/version.h" version_h_contents)
STRING(
        REGEX REPLACE
        ".*#define[ \\t]+VERSION[ \\t]+\"([^\"]*)\".*"
        "\\1" VERSION "${version_h_contents}"
)
IF("${VERSION}" STREQUAL "${version_h_contents}")
        MESSAGE(FATAL_ERROR "Failed to extract VERSION from version.h")
ENDIF()

# VERSION_QUAD must be either empty or be in the form of X.Y.Z.Y
STRING(
        REGEX REPLACE
        ".*#define[ \\t]+VERSION_QUAD[ \\t]+\"(([0-9]+\\.[0-9]+\\.[0-9]+\\.[0-9]+)?)\".*"
        "\\1" VERSION_QUAD "${version_h_contents}"
)
IF("${VERSION_QUAD}" STREQUAL "${version_h_contents}")
        MESSAGE(FATAL_ERROR "Failed to extract VERSION_QUAD from version.h")
ENDIF()

# For config.h
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_BINARY_DIR}")

INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFileCXX)
INCLUDE(TestCXXAcceptsFlag)
IF(UNIX AND NOT APPLE)
INCLUDE(cmake/FindCanberra.cmake)
ENDIF(UNIX AND NOT APPLE)
INCLUDE(cmake/SetDefaultBuildType.cmake)
INCLUDE(cmake/SetDefaultGccFlags.cmake)
INCLUDE(cmake/UpdateTranslations.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

ST_SET_DEFAULT_BUILD_TYPE(Release)
IF(CMAKE_COMPILER_IS_GNUCC)
        ST_SET_DEFAULT_GCC_FLAGS()
ENDIF(CMAKE_COMPILER_IS_GNUCC)

GET_FILENAME_COMPONENT(source_outer_dir "${PROJECT_SOURCE_DIR}/.." ABSOLUTE)
GET_FILENAME_COMPONENT(build_outer_dir "${PROJECT_BINARY_DIR}/.." ABSOLUTE)

INCLUDE(FindOpenGL)
SET(use_opengl OFF)
IF(OPENGL_FOUND AND QT_QTOPENGL_FOUND)
        # Disabled for now due to various problems associated with it.
        # SET(use_opengl ON)
ENDIF()
OPTION(ENABLE_OPENGL "OpenGL may be used for UI acceleration" ${use_opengl})

SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)

if(NOT DEFINED QtPkgList)
    set(QtPkgList "Qt5;Qt6")
endif()

foreach(QtPkg ${QtPkgList})
  if(ENABLE_OPENGL)
        find_package(QT NAMES ${QtPkg} HINTS "${QT_ROOT_DIR}" CONFIG REQUIRED COMPONENTS Core Widgets Gui Xml Network Svg LinguistTools OpenGL)
  else()
        find_package(QT NAMES ${QtPkg} HINTS "${QT_ROOT_DIR}" CONFIG REQUIRED COMPONENTS Core Widgets Gui Xml Network Svg LinguistTools)
  endif()

  if(QT_FOUND)
        break()
  endif()
endforeach()

if(ENABLE_OPENGL)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui Xml Network Svg LinguistTools OpenGL REQUIRED)
else()
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Gui Xml Network Svg LinguistTools REQUIRED)
endif()

SET(QT_MIN_VERSION 5.7.0)
if (QT_VERSION VERSION_LESS ${QT_MIN_VERSION})
        message(FATAL_ERROR "Minimum supported Qt version is ${QT_MIN_VERSION}!")
endif()

IF(APPLE)
   # Not used but required for loading cocoa plugin
   FIND_PACKAGE(Qt${QT_VERSION_MAJOR} COMPONENTS PrintSupport REQUIRED)
ENDIF(APPLE)

SET(EXTRA_LIBS "")

FIND_PACKAGE(JPEG REQUIRED)
INCLUDE_DIRECTORIES("${JPEG_INCLUDE_DIRS}")
LIST(APPEND EXTRA_LIBS ${JPEG_LIBRARIES})

OPTION(ENABLE_OPENJPEG "OpenJPEG library may be used to support JPEG 2000 (.jp2) images." ON)
IF (ENABLE_OPENJPEG)
        find_package(OPENJPEG REQUIRED)
        INCLUDE_DIRECTORIES("${OPENJPEG_INCLUDE_DIRS}")
        LIST(APPEND EXTRA_LIBS ${OPENJPEG_LIBRARIES})
ENDIF(ENABLE_OPENJPEG)

FIND_PACKAGE(ZLIB REQUIRED)
INCLUDE_DIRECTORIES("${ZLIB_INCLUDE_DIRS}")
LIST(APPEND EXTRA_LIBS ${ZLIB_LIBRARIES})

FIND_PACKAGE(PNG REQUIRED)
INCLUDE_DIRECTORIES("${PNG_INCLUDE_DIRS}")
LIST(APPEND EXTRA_LIBS ${PNG_LIBRARIES})

FIND_PACKAGE(TIFF REQUIRED)
INCLUDE_DIRECTORIES("${TIFF_INCLUDE_DIRS}")
LIST(APPEND EXTRA_LIBS ${TIFF_LIBRARIES})
IF(WIN32)
        ADD_DEFINITIONS(-DUSE_LIBTIFF_DLL)
ENDIF()

FIND_PACKAGE(Eigen3 REQUIRED)
INCLUDE_DIRECTORIES(SYSTEM ${EIGEN3_INCLUDE_DIR})
ADD_COMPILE_OPTIONS(-DEIGEN_MPL2_ONLY)
ADD_DEFINITIONS(-DEIGEN_DONT_ALIGN_STATICALLY)

IF(WIN32)
        SET(Boost_USE_STATIC_LIBS ON)
ELSE(WIN32)
        ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
ENDIF(WIN32)

ADD_DEFINITIONS(-DBOOST_MULTI_INDEX_DISABLE_SERIALIZATION)
ADD_DEFINITIONS(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

SET(Boost_USE_MULTITHREADED ON)
IF(ENABLE_TESTS)
        IF(POLICY CMP0167)
                FIND_PACKAGE(Boost 1.70.0 COMPONENTS unit_test_framework REQUIRED CONFIG)
        ELSE()
                FIND_PACKAGE(Boost 1.35.0 COMPONENTS unit_test_framework REQUIRED)
        ENDIF()
ELSE()
        IF(POLICY CMP0167)
                FIND_PACKAGE(Boost 1.70.0 REQUIRED CONFIG)
        ELSE()
                FIND_PACKAGE(Boost 1.35.0 REQUIRED)
        ENDIF()
ENDIF()
INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    include_directories(${OpenMP_CXX_INCLUDE_DIRS})
endif()

OPTION(ENABLE_EXIV2 "Exiv2 library may be used to manage image metadata." ON)
IF (ENABLE_EXIV2)
        find_package(EXIV2 REQUIRED)
        INCLUDE_DIRECTORIES(SYSTEM "${EXIV2_INCLUDE_DIRS}")
        LIST(APPEND EXTRA_LIBS ${EXIV2_LIBRARIES})
        ADD_DEFINITIONS(-DHAVE_EXIV2)
ENDIF()

IF(UNIX)
        find_package(Threads REQUIRED)
        LINK_LIBRARIES(Threads::Threads)
ELSEIF(WIN32)
        ADD_DEFINITIONS(-DNOMINMAX)
ENDIF(UNIX)

IF(CANBERRA_FOUND)
        ADD_DEFINITIONS(-DHAVE_CANBERRA)
        INCLUDE_DIRECTORIES("${CANBERRA_INCLUDE_DIRS}")
        LIST(APPEND EXTRA_LIBS ${CANBERRA_LIBRARIES})
ENDIF(CANBERRA_FOUND)

IF(WIN32)
   LIST(APPEND EXTRA_LIBS "ws2_32.lib")
ENDIF(WIN32)


SET(MAYBE_QT_OPENGL_MODULE "")
IF(ENABLE_OPENGL)
        LIST(APPEND EXTRA_LIBS ${OPENGL_LIBRARIES})
        SET(MAYBE_QT_OPENGL_MODULE Qt${QT_VERSION_MAJOR}::OpenGL)
ENDIF()

# Prepare config.h
IF(WIN32)
        SET(TRANSLATIONS_DIR_REL "translations")
        SET(STYLESHEETS_DIR_REL "stylesheets")
ELSEIF(APPLE)
        SET(TRANSLATIONS_DIR_REL "../Resources/translations")
        SET(STYLESHEETS_DIR_REL "../Resources/stylesheets")
ELSE()
        SET(TRANSLATIONS_DIR_REL "share/scantailor-deviant/translations")
        SET(STYLESHEETS_DIR_REL "share/scantailor-deviant/stylesheets")
        SET(PIXMAPS_DIR_ABS "${CMAKE_INSTALL_PREFIX}/share/scantailor-deviant/stylesheets")
ENDIF()
SET(TRANSLATIONS_DIR_ABS "${CMAKE_INSTALL_PREFIX}/${TRANSLATIONS_DIR_REL}")
SET(STYLESHEETS_DIR_ABS "${CMAKE_INSTALL_PREFIX}/${STYLESHEETS_DIR_REL}")


CONFIGURE_FILE(config.h.in ${CMAKE_BINARY_DIR}/config.h @ONLY)


ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(packaging)
