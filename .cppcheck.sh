#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later

cppcheck -DQTPOKIT_NAMESPACE=qtpockit -UQ_DECLARE_PRIVATE \
           --enable=all --error-exitcode=1 -Isrc \
           --suppress={missingInclude,unknownMacro} \
           --suppress=unusedFunction:test/unit/*.cpp \
           src test
