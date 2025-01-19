# SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
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
    add_custom_target(
      coverage
      COMMAND ${CODECOV_LCOV} --capture --directory . --gcov-tool ${CODECOV_GCOV} --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --list coverage.info
      # Linux/macOS
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info /usr/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info */.qt/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info */.rcc/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info */moc_*.cpp --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info */mocs_compilation.cpp --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info */qt/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info */Qt/* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info */test/* --output-file coverage.info
      # Windows
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info *:\\mingw32\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info *:\\mingw64\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info *\\.qt\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info *\\.rcc\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info *\\moc_*.cpp --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info *\\Qt\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info *\\test\\* --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --list coverage.info
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
