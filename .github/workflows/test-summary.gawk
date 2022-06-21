# SPDX-FileCopyrightText: 2022 Paul Colby <git@colby.id.au>
# SPDX-License-Identifier: LGPL-3.0-or-later
#
# Summarise a set of QTest TAP (Test Anything Protocol) output files as a
# GitHub Flavored Markdown table. Typical usage is something like:
#
# testFoo -o foo.tap,tap -o -,txt
# testBar -o bar.tap,tap -o -,txt
# ...
# gawk --file test-summary.gawk --sandbox *.tap >> $GITHUB_STEP_SUMMARY
#

BEGIN {
  RS="\r?\n"     # Allow both DOS and *nix line endings.
  IGNORECASE = 1 # Non-zero number means yes.
  maxNameLength = 0
}

FNR==2 {
  testName = $2
  if (length(testName) > maxNameLength)
    maxNameLength = length(testName)
  if (!(testName in tests)) {
    tests[testName]["skip"] = 0
    tests[testName]["pass"] = 0
    tests[testName]["fail"] = 0
  }
}

/^ok .*[^\\]#\s*SKIP/{
  tests[testName]["skip"]++
}

/^#\s*(pass|fail)\s+[0-9]+$/ {
  tests[testName][$2] += $3
}

END {
  nameDashes = "-"
  while (length(nameDashes) < maxNameLength) nameDashes = nameDashes "-"
  printf "|       Test result       | Passed | Failed | Skipped | %-*s |\n", maxNameLength, "Test name"
  printf "|:------------------------|-------:|-------:|--------:|:%*s-|\n", maxNameLength, nameDashes
  PROCINFO["sorted_in"] = "@ind_str_asc"
  for (name in tests) {
    printf "| %-18s %4s | %6u | %6u | %7u | %-*s |\n",
      (tests[name]["fail"]) ? ":x:" : ":heavy_check_mark:",
      (tests[name]["fail"]) ? "fail" : "pass",
      tests[name]["pass"] - tests[name]["skip"],
      tests[name]["fail"], tests[name]["skip"],
      maxNameLength, name
  }
}
