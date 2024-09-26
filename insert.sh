

#awk -i '/usr/share/awk/inplace.awk' -f versions.awk -- \
awk -f versions.awk -- \
  $(find . -mindepth 2 -name cov -prune -or -name '*.html' -print | head -n1)
