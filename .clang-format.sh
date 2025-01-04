for style in LLVM GNU Google Chromium Microsoft Mozilla WebKit; do
#for style in WebKit; do
  #git checkout include src >& /dev/null
  sed -i -Ee "s|^(BasedOnStyle:).*|\1 ${style}|" .clang-format
  clang-format -i include/qtpokit/*.h src/*/*.{cpp,h}
  printf "%-9s %5d\n" "${style}" "$(git diff | wc -l)"
done
