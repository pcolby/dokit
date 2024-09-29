#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later
#
# This script is used to inject a 'version selector' into each of the documentation versions, to make it easier to
# navigate between them. It does this by running AWK script (which does the real work) on every HTML file (except the
# the very top level `index.html` index). The AWK script replaces the (Doxygen or LCOV) HTML pages' project version
# a JavaScript and HTML snippet that renders select boxes for each of the published versions, with relevant version
# pre-selected. See also: `.selector.awk` and `.selector.html`

set -o errexit -o noclobber -o nounset -o pipefail

: "${AWKPATH:=/usr/share/awk}"

find . -mindepth 2 -name '*.html' | xargs -n 1000 -P 8 awk -f '.selector' -i 'inplace' --
