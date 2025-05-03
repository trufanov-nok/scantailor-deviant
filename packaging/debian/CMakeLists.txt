set(CPACK_GENERATOR "DEB")

set(CMAKE_INSTALL_PREFIX "/usr/")
SET(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

STRING(REPLACE ";" "" CPACK_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
SET(CPACK_DEBIAN_PACKAGE_SECTION graphics)
SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/ImageProcessing-ElectronicPublications/scantailor-deviant/")

set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

if (${QT_VERSION_MAJOR} STREQUAL "5")
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "qtbase${QT_VERSION_MAJOR}-dev (>=5.4), libqt${QT_VERSION_MAJOR}svg${QT_VERSION_MAJOR} (>=5.4), qttools${QT_VERSION_MAJOR}-dev (>=5.4)")
else()
        if(${QT_VERSION_MAJOR} STREQUAL "6")
                set(CPACK_DEBIAN_PACKAGE_DEPENDS "qt${QT_VERSION_MAJOR}-base-dev (>=6), libqt${QT_VERSION_MAJOR}svg${QT_VERSION_MAJOR} (>=6)")
        endif()
endif()

set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libtiff-dev (>=4.0.3), libjpeg8-dev (>=8c), libpng-dev (>=1.6.2), libboost-dev (>=1.35.0), libboost-test-dev (>=1.35.0), cmake (>=2.8.9), libcanberra0(>=0.30), libopenjp2-7 (>=2.3), libexiv2-dev (>=0.26)")

if (OPENMP_FOUND)
        set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libomp5 | libomp5-16")
endif()

INSTALL(FILES scantailor-deviant.svg DESTINATION "share/pixmaps/")
INSTALL(FILES scantailor-deviant.desktop DESTINATION "share/applications/")

INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/src/stylesheets" DESTINATION "share/scantailor-deviant")

FIND_PROGRAM(GZIP gzip REQUIRED)
EXECUTE_PROCESS(
        COMMAND "${GZIP}" "-9n"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/changelog"
        OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"
        RESULT_VARIABLE COMMAND_ERROR)
IF(COMMAND_ERROR)
        MESSAGE(FATAL_ERROR "Could not gzip changelog")
ENDIF()

INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/changelog.gz"     DESTINATION "share/doc/scantailor-deviant")
INSTALL(FILES "${CMAKE_CURRENT_SOURCE_DIR}/copyright" DESTINATION "share/doc/scantailor-deviant")

include(CPack)
