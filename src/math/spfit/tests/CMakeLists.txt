INCLUDE_DIRECTORIES(BEFORE ..)

SET(
        sources
        ${CMAKE_SOURCE_DIR}/src/core/tests/main.cpp
        TestSqDistApproximant.cpp
)

SOURCE_GROUP("Sources" FILES ${sources})

SET(
        libs
        math ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        ${Boost_PRG_EXECUTION_MONITOR_LIBRARY} ${EXTRA_LIBS}
)

ADD_EXECUTABLE(spfit_tests ${sources})
TARGET_LINK_LIBRARIES(spfit_tests ${libs})

# We want the executable located where we copy all the DLLs.
SET_TARGET_PROPERTIES(
        spfit_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

ADD_TEST(NAME spfit_tests COMMAND spfit_tests --log_level=message)
