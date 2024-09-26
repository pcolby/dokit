find . -mindepth 2 -name cov -prune -or -name '*.html' -print |
  xargs -n 1000 -P 8 awk -f versions.awk -i '/usr/share/awk/inplace.awk' --
