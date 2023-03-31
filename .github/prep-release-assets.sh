#!/usr/bin/env bash
#
# Usage: go.sh <tag>

set -o errexit -o noclobber -o nounset -o pipefail # or set -Ceuo pipefail
shopt -s extglob inherit_errexit

: "${REPO_SLUG:=pcolby/pokit}"
: "${OUTPUT_DIR:=output}"

runId=$(gh run list -R "$REPO_SLUG" -b "$1" -w 'Build and Test' --json databaseId --jq '.[].databaseId')
gh run download "$runId" -R "$REPO_SLUG"

mkdir -p "$OUTPUT_DIR"

ls -1d qtpokit-* | grep -Eve '(-cov|\.win)\.|\.AppImage$' |
  xargs -I '{}' tar -cJvf "$OUTPUT_DIR/{}.txz" '{}'

ls -1d qtpokit-*.win.* | grep -Eve '-cov\.' |
  xargs -I '{}' zip -r "$OUTPUT_DIR/{}.zip" '{}'

zip -r "$OUTPUT_DIR/test-results.zip" coverage-report test-results-*

find . -type f -name '*.AppImage' -not -name '*-cov.*AppImage' \
  -not -path '*/output/*' -exec cp -av {} "$OUTPUT_DIR" \;
