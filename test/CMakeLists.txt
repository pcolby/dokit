# SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

if(BUILD_TESTING)
add_subdirectory(unit)
endif()

if (ENABLE_COVERAGE)
  find_program(CODECOV_LCOV lcov)
  find_program(CODECOV_GENHTML genhtml)
  if (CODECOV_LCOV)
    message("-- Found lcov ${CODECOV_LCOV}")
    add_custom_target(
      coverage
      COMMAND ${CODECOV_LCOV} --capture --directory . --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --list coverage.info
      COMMAND ${CODECOV_LCOV} --remove coverage.info '/usr/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --remove coverage.info '*/moc_*.cpp' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --remove coverage.info '*/qt/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --remove coverage.info '*/Qt/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --remove coverage.info '*/test/*' --output-file coverage.info
      COMMAND ${CODECOV_LCOV} --list coverage.info
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
  if (CODECOV_GENHTML)
    message("-- Found genhtml ${CODECOV_GENHTML}")
    add_custom_target(
      coverage-html
      COMMAND ${CODECOV_GENHTML} --output-directory coverage coverage.info
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
  endif()
  configure_file(removeHtmlDates.sh.in ${PROJECT_BINARY_DIR}/removeHtmlDates.sh)
endif()
