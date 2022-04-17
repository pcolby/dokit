#!/usr/bin/env bash

cppcheck -DQTPOKIT_NAMESPACE=qtpockit -UQ_DECLARE_PRIVATE \
           --enable=all --error-exitcode=1 -Isrc \
           --suppress={missingInclude,unknownMacro} \
           --suppress=unusedFunction:test/unit/*.cpp \
           src test
