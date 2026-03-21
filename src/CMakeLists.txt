# SPDX-FileCopyrightText: 2022-2026 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

set(TranslationSourceFiles
  i18n/en_AU.ts
  i18n/en_GB.ts
  i18n/en_US.ts)

add_subdirectory(cli)
add_subdirectory(lib)

# Experimental (for now) GUI app, off by default.
option(ENABLE_GUI "Enable test coverage reporting" OFF)
if (ENABLE_GUI)
  message(STATUS "Enabling experimental GUI app")
  add_subdirectory(gui)
endif()
