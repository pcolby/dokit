#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2022-2025 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later
set -o errexit -o noclobber -o nounset -o pipefail

cppcheck --version
RC=0
for platform in native unix{32,64} win{32{A,W},64} unspecified; do
  for defines in -{D,U}QTPOKIT_LIBRARY' '-{DQTPOKIT_NAMESPACE=qtpokit,UQTPOKIT_NAMESPACE}' '-{DPROJECT_BUILD_ID=\"1234\",UPROJECT_BUILD_ID}' '-{DPROJECT_PRE_RELEASE=pre,UPROJECT_PRE_RELEASE}; do
    echo "defines: ${platform} ${defines}"
    read -a definesArray -r <<< "${defines}"
    cppcheck -UQ_DECLARE_PRIVATE "${definesArray[@]}" \
      --enable=all --error-exitcode=1 -Isrc \
      --language=c++ --platform="${platform}" --std=c++{17,20} \
      --suppress={checkersReport,missingInclude,missingIncludeSystem,unknownMacro} \
      --suppress=unusedFunction:src/gui/resources.cpp \
      --quiet "$@" src test
    RC=$((RC+$?))
  done
done
exit "${RC}"
