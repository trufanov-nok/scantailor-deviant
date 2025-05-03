set(CPACK_GENERATOR "DEB")

SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION
                "Scan Tailor is an interactive post-processing tool for scanned pages.\n"
                " It performs operations such as page splitting, deskewing, adding/removing\n"
                " borders, and others. You give it raw scans, and you get pages ready to be\n"
                " printed or assembled into a PDF or DJVU file. Scanning, optical character\n"
                " recognition, and assembling multi-page documents are out of scope of this\n"
                " project.")
STRING(REPLACE ";" "" CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_DEBIAN_PACKAGE_DESCRIPTION})
SET(CPACK_DEBIAN_PACKAGE_SECTION graphics)
SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/ImageProcessing-ElectronicPublications/scantailor-deviant/")




INSTALL(FILES debian/scantailor-deviant.svg DESTINATION "share/pixmaps/")
INSTALL(FILES debian/scantailor-deviant.desktop DESTINATION "share/applications/")

INSTALL(DIRECTORY "${CMAKE_SOURCE_DIR}/src/stylesheets" DESTINATION "share/scantailor-deviant")

FIND_PROGRAM(GZIP gzip REQUIRED)
EXECUTE_PROCESS(
        COMMAND "${GZIP}" "-9n"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        INPUT_FILE "${CMAKE_SOURCE_DIR}/debian/changelog"
        OUTPUT_FILE "changelog.gz"
        RESULT_VARIABLE COMMAND_ERROR)
IF(COMMAND_ERROR)
        MESSAGE(FATAL_ERROR "Could not gzip changelog")
ENDIF()

INSTALL(FILES "${CMAKE_BINARY_DIR}/changelog.gz"     DESTINATION "share/doc/scantailor-deviant")
INSTALL(FILES "${CMAKE_SOURCE_DIR}/debian/copyright" DESTINATION "share/doc/scantailor-deviant")

