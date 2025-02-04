@load "readfile"

BEGIN {
  selector = readfile(".selector.html")
  if (!selector) {
    print "Failed to read selector file: .selector.html" > "/dev/stderr"
    exit 1
  }
}

BEGINFILE {
  # Extract the version number from the file path.
  if (match(FILENAME, "^\\./([0-9.]{5,}|main)/(cov|doc|int)/", parts) == 0) {
    print "Failed to extract version info from file:", FILENAME > "/dev/stderr"
    exit 2
  }
  version = parts[1]
  visibility = parts[2]
}

# Doxygen project number.
match($0, /(^.* id="projectnumber">)(.*<\/span>)?$/, parts) {
  printf "%s\n%s   </span>\n", parts[1], getSelector()
  skip = 1 # Begin skipping (until the next </div>).
}

# LCOV project version.
match(prev, /<td class="headerItem">Version:<\/td>/) {
  if (match($0, "^.*class=\"headerValue\">", parts) == 0) {
    print "Failed to recognise version header:", $0 > "/dev/stderr"
    exit 3
  }
  printf "%s\n%s            </td>\n", parts[0], getSelector()
  skip = 1 # Begin skipping (until the next <td></td>).
}

# Return `selector` with the current version option selected.
function getSelector() {
  # Figure out how many "../" entries are required.
  split(FILENAME, parts, "/")
  ancestors = ""
  for (i = length(parts); i > 2; --i)
    ancestors = ancestors"../"
  thisSelector = gensub("\\.\\./\\.\\./", ancestors, "g", selector)

  # Increase the indentation in LCOV files (purely for aesthetics).
  if (visibility == "cov")
    thisSelector = gensub(" {4}", "&&& ", "g", thisSelector)

  # Select the current version.
  pattern = "value=\"("gensub(/\./, "\\\\.", "g", version)"|"visibility")\""
  #printf "version: %s\nvisibility: %s\npattern: %s\n", version, visibility, pattern
  return gensub(pattern, "\\0 selected", "g", thisSelector)
}

/<\/div>|<td><\/td>/ { skip = 0 }

!skip { print }

{ prev = $0 }
