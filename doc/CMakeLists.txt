# SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

# Include Qt doc tagfiles, if provided.
if (QT_TAGFILES_PATH)
  set(QT_DOCS_BASEURL "https://doc.qt.io/qt-${QT_VERSION_MAJOR}/")
  message("-- Using Qt doc tagfiles ${QT_TAGFILES_PATH} (${QT_DOCS_BASEURL})")
  set(DOX_TAGFILES "\\
  \"${QT_TAGFILES_PATH}/qtcore/qtcore.tags=${QT_DOCS_BASEURL}\" \\
  \"${QT_TAGFILES_PATH}/qtbluetooth/qtbluetooth.tags=${QT_DOCS_BASEURL}\" \\")
endif()

find_package(Doxygen)
if (DOXYGEN_FOUND)
  # Public docs.
  unset(DOX_ENABLED_SECTIONS)
  set(DOX_HTML_OUTPUT_DIR public)
  set(DOX_INPUT_DOC "doc/src/public")
  set(DOX_INPUT_SRC "src/lib")
  set(DOX_EXCLUDE_PATTERNS "*_p.h")
  set(DOX_EXCLUDE_SYMBOLS "*Private")
  set(DOX_INCLUDE_INTERNAL NO)
  set(DOX_PROJECT_BRIEF "Native Qt library for Pokit devices")
  configure_file(doxyfile.in public.doxy)
  add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} public.doxy)

  # Internal docs.
  set(DOX_ENABLED_SECTIONS internal)
  set(DOX_HTML_OUTPUT_DIR internal)
  set(DOX_INPUT_DOC "doc/src/internal")
  set(DOX_INPUT_SRC "src")
  unset(DOX_EXCLUDE_PATTERNS)
  unset(DOX_EXCLUDE_SYMBOLS)
  set(DOX_INCLUDE_INTERNAL YES)
  set(DOX_PROJECT_BRIEF "Internal development documentation")
  configure_file(doxyfile.in internal.doxy)
  add_custom_target(doc-internal COMMAND ${DOXYGEN_EXECUTABLE} internal.doxy)
endif()
