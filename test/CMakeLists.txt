# SPDX-FileCopyrightText: 2022-2026 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
  # Initialise an empty list (string).
  set(LLVM_PROFILE_FILES "" CACHE INTERNAL "Internal")
endif()

if(BUILD_TESTING)
add_subdirectory(unit)
endif()

if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "Clang$")
  find_program(LLVM_PROFDATA llvm-profdata)
  find_program(LLVM_COV llvm-cov)
  if (LLVM_PROFDATA AND LLVM_COV)
    message(STATUS "Found llvm-profdata ${LLVM_PROFDATA}")
    message(STATUS "Found llvm-cov ${LLVM_COV}")
    file(
      GLOB_RECURSE LLVM_COV_SOURCES
      LIST_DIRECTORIES false
      "${PROJECT_SOURCE_DIR}/include/*.h"
      "${PROJECT_SOURCE_DIR}/src/*.cpp"
      "${PROJECT_SOURCE_DIR}/src/*.h")
    if (APPLE AND CMAKE_OSX_ARCHITECTURES MATCHES ".*;.*")
      list(GET CMAKE_OSX_ARCHITECTURES 0 LLVM_COV_ARCH)
    endif()
    add_custom_target(
      coverage
      COMMAND ${LLVM_PROFDATA} merge --output coverage.profdata "${LLVM_PROFILE_FILES}"
      COMMAND ${LLVM_PROFDATA} show coverage.profdata
      COMMAND ${LLVM_COV} export
        "$<$<BOOL:${LLVM_COV_ARCH}>:--arch;${LLVM_COV_ARCH}>"
        --format lcov --instr-profile coverage.profdata
        "--object;$<JOIN:$<TARGET_OBJECTS:QtPokit>,;--object;>"
        "$<$<TARGET_EXISTS:cli-lib>:--object;$<JOIN:$<TARGET_OBJECTS:cli-lib>,;--object;>>"
        --sources "${LLVM_COV_SOURCES}" > coverage.info
      COMMAND_EXPAND_LISTS
      COMMENT "Collating coverage data"
      VERBATIM
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
endif()

if (ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  find_program(CODECOV_GCOV gcov)
  find_program(CODECOV_LCOV lcov)
  if (CODECOV_LCOV)
    message(STATUS "Found gcov ${CODECOV_GCOV}")
    message(STATUS "Found lcov ${CODECOV_LCOV}")

    # Determine the LCOV version.
    execute_process(
      COMMAND ${CODECOV_LCOV} --version
      OUTPUT_VARIABLE LCOV_VERSION_STRING
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(DEBUG "LCOV: version string: ${LCOV_VERSION_STRING}")
    string(REGEX MATCH "^(.*version )?(.+)$" _ "${LCOV_VERSION_STRING}")
    set(LCOV_VERSION "${CMAKE_MATCH_2}")
    message(VERBOSE "LCOV: version: ${LCOV_VERSION}")

    # Build the LCOV --ignore-errors list.
    set(ignoreErrors --ignore-errors gcov,gcov)
    if ("${LCOV_VERSION}" VERSION_GREATER_EQUAL 2)
      # 'inconsistent' was added to --ignore-errors in LCOV 2.0.
      set(ignoreErrors "${ignoreErrors},inconsistent,inconsistent")
    endif()
    set(removeCoverage "${ignoreErrors},unused,unused" --remove coverage.info)
    message(DEBUG "LCOV: ignoreErrors: ${ignoreErrors}")
    message(DEBUG "LCOV: removeCoverage: ${removeCoverage}")

    add_custom_target(
      coverage
      COMMAND ${CODECOV_LCOV} --capture --directory . --gcov-tool ${CODECOV_GCOV} ${ignoreErrors} --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${ignoreErrors} --list coverage.info
      # Linux/macOS
      COMMAND ${CODECOV_LCOV} ${removeCoverage} /usr/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} */.qt/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} */.rcc/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} */moc_*.cpp --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} */mocs_compilation.cpp --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} */qt/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} */Qt/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} */test/* --output-file coverage.info
      # Windows
      COMMAND ${CODECOV_LCOV} ${removeCoverage} *:\\mingw32\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} *:\\mingw64\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} *\\.qt\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} *\\.rcc\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} *\\moc_*.cpp --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} *\\Qt\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${removeCoverage} *\\test\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} ${ignoreErrors} --list coverage.info
      COMMENT "Collating coverage data"
      VERBATIM
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
endif()

if (ENABLE_COVERAGE)
  find_program(CODECOV_GENHTML genhtml)
  if (CODECOV_GENHTML)
    message(STATUS "Found genhtml ${CODECOV_GENHTML}")
    add_custom_target(
      coverage-html
      COMMAND ${CODECOV_GENHTML} --output-directory coverage --prefix ${PROJECT_SOURCE_DIR} coverage.info
      COMMENT "Generating HTML coverage report"
      VERBATIM
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
  configure_file(removeHtmlDates.sh.in ${PROJECT_BINARY_DIR}/removeHtmlDates.sh @ONLY)
endif()
