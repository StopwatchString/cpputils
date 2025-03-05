cmake_minimum_required(VERSION 3.30)

# Globals
set(PROJECT_NAME "cpputils project")
set(TARGET_NAME "cpputils")

project(${PROJECT_NAME})

# Environment settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# List all project source in these variables
set(SOURCES)
set(PRIVATE_HEADERS)
set(PUBLIC_HEADERS
    ${CMAKE_SOURCE_DIR}/include/cpputils/InterprocessMutex.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/InterprocessMutexLockGuard.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/languageChecking.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/SharedLibraryLoader.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/SharedMemory.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/Timer.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/WaitFreeCounter.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/WriteSafeLockFree.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/WriteUnsafeLockFree.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/windows/credential_utils.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/windows/dwm.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/windows/handle_utils.h
    ${CMAKE_SOURCE_DIR}/include/cpputils/windows/selectors.h
)
# List all GTest files here
# Generally every hpp/cpp combo should have a matching [filename].test.cpp.
# Header-only should also receive test files.
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/src/cpputils/SharedMemory.test.cpp
)

# -------------- PROJECT LIBRARY --------------
# Create linkable library from project files
# We break this into a library so that the main exe and test project
# share the same artefacts
add_library(${TARGET_NAME}_lib INTERFACE ${PUBLIC_HEADERS})
target_include_directories(${TARGET_NAME}_lib
    INTERFACE ${CMAKE_SOURCE_DIR}/include
)

# ------------- PROJECT EXECUTABLE -------------
add_executable(${TARGET_NAME} src/main/main.cpp)
target_link_libraries(${TARGET_NAME} PRIVATE ${TARGET_NAME}_lib)

# ------------------ TESTING ------------------
# Add Google Test
add_subdirectory(external/googletest)

enable_testing()

add_executable(${TARGET_NAME}_tests ${TEST_SOURCES})
target_link_libraries(
    ${TARGET_NAME}_tests
    PRIVATE GTest::gtest_main
    PRIVATE ${TARGET_NAME}_lib
)
target_include_directories(
    ${TARGET_NAME}_tests
    # Explicitly access all headers, regardless of visibility from _lib
    PRIVATE ${CMAKE_SOURCE_DIR}/include
    PRIVATE ${CMAKE_SOURCE_DIR}/src
)

include(GoogleTest)
gtest_discover_tests(${TARGET_NAME}_tests)

# --------------- MSVC Settings ---------------
if (MSVC)
    # Organize Google Test targets into a subfolder in Visual Studio
    set_target_properties(gtest gtest_main gmock gmock_main
        PROPERTIES FOLDER "GoogleTest"
    )

    # Set Visual Studio startup project as our main target (normally VS will default to BUILD_ALL)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

    # Enable Multiprocessor compile on targets
    #target_compile_options(${TARGET_NAME}_lib PRIVATE /MP)
    target_compile_options(${TARGET_NAME} PRIVATE /MP)
    target_compile_options(${TARGET_NAME}_tests PRIVATE /MP)

    # Provides folder tree in visual studio filters
    source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
    source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Private Header Files" FILES ${PRIVATE_HEADERS})
    source_group(TREE ${CMAKE_SOURCE_DIR}/include PREFIX "Public Header Files" FILES ${PUBLIC_HEADERS})
endif()