# SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

if(BUILD_TESTING)
add_subdirectory(unit)
endif()

if (ENABLE_COVERAGE)
  find_program(CODECOV_GCOV gcov)
  find_program(CODECOV_LCOV lcov)
  find_program(CODECOV_GENHTML genhtml)
  if (CODECOV_LCOV)
    message(STATUS "Found gcov ${CODECOV_GCOV}")
    message(STATUS "Found lcov ${CODECOV_LCOV}")
    add_custom_target(
      coverage
      COMMAND ${CODECOV_LCOV} --capture --directory . --gcov-tool ${CODECOV_GCOV} --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --list coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '/usr/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '*/.qt/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '*/.rcc/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '*/moc_*.cpp' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '*/mocs_compilation.cpp' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '*/qt/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '*/Qt/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --ignore-errors unused --remove coverage.info '*/test/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --list coverage.info
      COMMENT "Collating coverage data"
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
  if (CODECOV_GENHTML)
    message(STATUS "Found genhtml ${CODECOV_GENHTML}")
    add_custom_target(
      coverage-html
      COMMAND ${CODECOV_GENHTML} --output-directory coverage coverage.info
      COMMENT "Generating HTML coverage report"
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
  configure_file(removeHtmlDates.sh.in ${PROJECT_BINARY_DIR}/removeHtmlDates.sh)
endif()
