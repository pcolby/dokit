@load "readfile"

BEGIN {
  versions = readfile("versions.inc")
}

BEGINFILE {
  # Extract the version number from the file path.
  if (match(FILENAME, "^\\./([0-9.]{5,}|main)/(cov|doc|int)/", parts) == 0) {
    print "Failed to extract version info from:" FILENAME > "/dev/stderr"
    exit 1
  }
  version = parts[1]
  visibility = parts[2]
}

# Doxygen project number.
match($0, /(^.* id="projectnumber">)(.*<\/span>)?$/, parts) {
  # Inject the version <select> elements, selecting the current options first.
  pattern = "value=\"("gensub(/\./, "\\\\.", "g", version)"|"visibility")\""
  #printf "version: %s\nvisibility: %s\npattern: %s\n", version, visibility, pattern
  printf "%s\n%s   </span>\n", parts[1], gensub(pattern, "\\0 selected", "g", versions)
  skip = 1 # Begin skipping (until the next </div>).
}

# LCOV project version.
match(prev, /<td class="headerItem">Version:<\/td>/) {
  split(FILENAME, parts, "/")
  ancestors = ""
  for (i = length(parts); i > 2; --i)
    ancestors = ancestors"../"
  if (match($0, "^.*class=\"headerValue\">", parts) == 0) {
    print "Failed to recognise version header:" $0 > "/dev/stderr"
    exit 2
  }
  # Inject the version <select> elements, selecting the current options first.
  pattern = "value=\"("gensub(/\./, "\\\\.", "g", version)"|"visibility")\""
  #printf "version: %s\nvisibility: %s\npattern: %s\n", version, visibility, pattern
  thisVersions = gensub(pattern, "\\0 selected", "g", versions)
  thisVersions = gensub("\\.\\./\\.\\./", ancestors, "g", thisVersions)
  printf "%s\n%s   </td>\n", parts[0], thisVersions
  skip = 1 # Begin skipping (until the next <td></td>).
}

/<\/div>|<td><\/td>/ { skip = 0 }

!skip { print }

{ prev = $0 }
