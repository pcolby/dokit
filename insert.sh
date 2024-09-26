#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2022-2024 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

set -o errexit -o noclobber -o nounset -o pipefail

: "${AWKPATH:=/usr/share/awk}"

find . -mindepth 2 -name cov -prune -or -name '*.html' -print |
  xargs -n 1000 -P 8 awk -f 'versions' -i 'inplace' --
