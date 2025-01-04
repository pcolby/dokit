#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

set -o errexit -o noclobber -o nounset -o pipefail
shopt -s extglob globstar inherit_errexit

clang-format --version

for style in LLVM GNU Google Chromium Microsoft Mozilla WebKit; do
  #git checkout include src >& /dev/null
  sed -i -Ee "s|^(BasedOnStyle:).*|\1 ${style}|" .clang-format
  clang-format --dump-config >| .clang-format-dump-$style
  clang-format -i **/*.{cpp,h}
  printf "%-9s %5d\n" "${style}" "$(git diff | wc -l)"
done
