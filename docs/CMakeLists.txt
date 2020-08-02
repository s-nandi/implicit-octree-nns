find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

# Find public headers
get_target_property(${PROJECT_NAME}_PUBLIC_HEADER_DIR
        ${PROJECT_NAME}
        INTERFACE_INCLUDE_DIRECTORIES)
file(GLOB_RECURSE
        ${PROJECT_NAME}_PUBLIC_HEADERS
        ${${PROJECT_NAME}_PUBLIC_HEADER_DIR}/*.h
        ${${PROJECT_NAME}_PUBLIC_HEADER_DIR}/*.hpp)

# Generate doxygen documentation
set(DOXYGEN_INPUT_DIR "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}")
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/html/index.html)

set(DOXYFILE_IN Doxyfile.in)
set(DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/doxyfile)

configure_file(${DOXYFILE_IN} ${DOXYFILE} @ONLY)

file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})
add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE}
        COMMENT "Generating Doxygen documentation"
        DEPENDS ${${PROJECT_NAME}_PUBLIC_HEADERS}
        MAIN_DEPENDENCY ${DOXYFILE_IN} ${DOXYFILE}
        VERBATIM)

set(DOXYGEN_TARGET_NAME ${PROJECT_NAME}_doxygen)
add_custom_target(${DOXYGEN_TARGET_NAME} ALL
        DEPENDS ${DOXYGEN_INDEX_FILE})

# Use doxygen's output to build sphinx documentation
set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
set(SPHINX_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/sphinx)

file(MAKE_DIRECTORY ${SPHINX_OUTPUT_DIR})
set(SPHINX_TARGET_NAME ${PROJECT_NAME}_sphinx)
add_custom_target(${SPHINX_TARGET_NAME} ALL
        COMMAND ${SPHINX_EXECUTABLE} -b html
        -Dbreathe_projects.${PROJECT_NAME}=${DOXYGEN_OUTPUT_DIR}/xml
        ${SPHINX_SOURCE} ${SPHINX_OUTPUT_DIR}
        COMMENT "Generating Sphinx documentation"
        MAIN_DEPENDENCY source/conf.py
        DEPENDS
        ${DOXYGEN_TARGET_NAME}
        # List documentation files below
        source/index.rst)