#!/usr/bin/env bash
set -o errexit -o noclobber -o nounset -o pipefail

: "${ARTIFACTS_DIR:=$(realpath -m artifacts)}"
: "${ASSETS_DIR:=$(realpath -m assets)}"
readonly ARTIFACTS_DIR ASSETS_DIR TAG_NAME="${1:?Usage: ${BASH_SOURCE[0]} <tag-name>}"
[[ ! -a "${ARTIFACTS_DIR}" && ! -a "${ASSETS_DIR}" ]] || {
  echo 'One or more output directories already exist.'
  printf 'Please delete the following directories to avoid errors:\n  %s\n  %s\n' "${ARTIFACTS_DIR}" "${ASSETS_DIR}"
  exit 2
}
printf 'Creating output directories:\n  %s\n  %s\n' "${ARTIFACTS_DIR}" "${ASSETS_DIR}"
mkdir -p "${ARTIFACTS_DIR}" "${ASSETS_DIR}"

# Fetch the workflow runs for the tag.
echo "Fetching workflow run info for: ${TAG_NAME}"
runsList=$(gh run list --branch "${TAG_NAME}" --json 'name,number,databaseId,status,url,displayTitle')
[[ -n "${runsList}" && "${runsList}" != '[]' ]] || {
  echo "Found no workflow runs for: ${TAG_NAME}"
  exit 3
}
title=$(jq --raw-output 'first.displayTitle' <<< "${runsList}")
echo "Found workflow run info: ${title}"

# Check all workflows have completed.
incomplete=$(jq --raw-output '.[]|select(.status != "completed")|[" ",.url,.name,.status]|join(" - ")' <<< "${runsList}")
[[ -z "${incomplete}" ]] || {
  printf 'The following workflows are incomplete:\n%s\n' "${incomplete}"
  exit 4
}

# Fetch the build artifacts.
buildRunId=$(jq --raw-output '.[]|select(.name == "Build and Test").databaseId' <<< "${runsList}")
docRunId=$(jq --raw-output '.[]|select(.name == "Documentation").databaseId' <<< "${runsList}")
[[ -n "${buildRunId}" && -n "${docRunId}" ]] || {
  echo "Failed to find run IDs for build (${buildRunId}) and/or doc (${docRunId}) workflow runs."
  exit 5
}

echo "Fetching artifacts for build run: ${buildRunId}"
gh run download "${buildRunId}" --dir "${ARTIFACTS_DIR}"

echo "Fetching artifacts for docs run ${docRunId}"
gh run download "${docRunId}" --dir "${ARTIFACTS_DIR}"

# Process the build artifacts into release assets.
echo 'Compressing Linux and macOS binaries'
find "${ARTIFACTS_DIR}" -maxdepth 1 -type d -name 'dokit-*' \
  -not -name '*-cov.*' -not -name '*.win.*' -not -name '*.AppImage' \
  -execdir tar -cJvf "${ASSETS_DIR}/{}.txz" --transform 's|^\./||' '{}' \;

echo 'Compressing Windows binaries'
find "${ARTIFACTS_DIR}" -maxdepth 1 -type d -name 'dokit-*.win.*' -not -name '*-cov.*' \
  -execdir zip -9 -r "${ASSETS_DIR}/{}.zip" '{}' \;

echo 'Compressing test results'
( cd "${ARTIFACTS_DIR}"; zip -9 -r "${ASSETS_DIR}/test-results.zip" 'coverage-report' 'test-results-'* )

echo 'Copying AppImages'
find "${ARTIFACTS_DIR}" -type f -name '*.AppImage' -not -name '*-cov.*AppImage' -exec cp -av {} "${ASSETS_DIR}" \;

echo 'Compressing docs'
find "${ARTIFACTS_DIR}" -maxdepth 1 -type d -name '*-docs' -execdir zip -9 -r "${ASSETS_DIR}/{}.zip" '{}' \;

# Create a new (draft) release.
echo "Creating new draft release, with $(ls -1a "${ASSETS_DIR}" | wc -l) assets"
lin=$(find "$ASSETS_DIR" -name '*.gcc.*.AppImage'                 -printf '%f\n' | sort -V | tail -n1)
mac=$(find "$ASSETS_DIR" -name '*.macos-*.arm64-x86_64.clang.*'   -printf '%f\n' | sort -V | tail -n1)
win=$(find "$ASSETS_DIR" -name '*.win.x86-64.msvc.*.portable.zip' -printf '%f\n' | sort -V | tail -n1)
readonly baseUrl="https://github.com/pcolby/dokit/releases/download/${TAG_NAME}" lin mac win
gh release create "${TAG_NAME}" --draft --generate-notes --notes-file <(cat <<--
	_Add release notes here._

	### Recommended Downloads

	Feel free to download any of the assets below, but the recommended starting point would be:

	* Linux: [${lin}](${baseUrl}/${lin})
	* macOS: [${mac}](${baseUrl}/${mac}) - tip: \`brew install qt6\`
	* Windows: [${win}](${baseUrl}/${win}) - tip: run the included \`vc_redist.x64.exe\` if you don't have the necessary Microsoft Visual C++ runtime installed already.
	-
) --verify-tag "${ASSETS_DIR}/"*
echo 'Done'
