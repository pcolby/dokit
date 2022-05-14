#!/usr/bin/env bash

# \todo Remove this soon.

set -o errexit -o noclobber -o nounset -o pipefail

declare -a appClassNames
declare -a libClassNames
while IFS= read -d '' -r pathName; do
  if [[ "$pathName" == */app/* ]]; then
    className=$(sed -Ene 's/^class *([a-zA-Z]+) *:.*/\1/p' "$pathName")
    appClassNames+=("$className")
  else
    className=$(sed -Ene 's/^class [A-Z_]* ([a-zA-Z]+)Private *:.*/\1/p' "$pathName")
    libClassNames+=("$className")
  fi
  testSourceFileName="test${className,,}.cpp"
  testHeaderFileName="test${className,,}.h"
  echo "$pathName::$className => $testSourceFileName $testHeaderFileName"
  if [[ ! -e "$testSourceFileName" ]]; then
    cp -a testxxx.cpp "$testSourceFileName"
    cp -a testxxx.h "$testHeaderFileName"
    sed -i -Ee "s/Xxx/$className/g" "$testSourceFileName" "$testHeaderFileName"
    sed -i -Ee "s/xxx/${className,,}/g" "$testSourceFileName" "$testHeaderFileName"
    if [[ "$pathName" == */app/* ]]; then
      sed -i -Ee 's|<qtpokit/(.*\.h)>|"\1"|g' "$testSourceFileName"
    fi
  fi
done < <(find ../../src -type f -name '*.h' -print0)

for className in $(echo "${libClassNames[@]}" | tr ' ' '\n' | sort); do
  echo -e "add_pokit_unit_test(\n  $className\n  test${className,,}.cpp\n  test${className,,}.h)\n"
done

for className in $(echo "${appClassNames[@]}" | tr ' ' '\n' | sort); do
  echo -e "add_pokit_app_unit_test(\n  $className\n  test${className,,}.cpp\n  test${className,,}.h)\n"
done
