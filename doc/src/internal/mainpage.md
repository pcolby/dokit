# QtPokit - Internal

This is *internal* documentation, aimed at developers that contribute to the QtPokit project itself.

## Building from Source

Prototypical [CMake][]-based out-of-source build and test process:

~~~{.sh}
cmake -E make_directory <tmp-build-dir>
cmake -D CMAKE_BUILD_TYPE=Release -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir>
ctest --test-dir <tmp-build-dir> --verbose
~~~

### Test Coverage

Similar to above, but specify `ENABLE_COVERAGE=YES`, which will add dependencies on [gcov][] and [LCOV][].

~~~{.sh}
cmake -E make_directory <tmp-build-dir>
cmake -D CMAKE_BUILD_TYPE=Release -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir>
ctest --test-dir <tmp-build-dir> --verbose
cmake --build <tmp-build-dir> --target coverage coverage-html
# ls <tmp-build-dir>/test/coverage.info # Coverage tracefile.
# ls <tmp-build-dir>/test/coverage/ # HTML report.
~~~

### Documentation

Configure the same as above, but build the `doc` and (optionally) `doc-internal` targets.

~~~{.sh}
cmake -E make_directory <tmp-build-dir>
cmake -S <path-to-cloned-repo> -B <tmp-build-dir>
cmake --build <tmp-build-dir> --target doc doc-internal
# ls <tmp-build-dir>/doc/public    # Library end-user documentation
# ls <tmp-build-dir>/doc/internal  # Internal developer documentation
~~~

[CMake]: https://cmake.org/
[gcov]:  https://gcc.gnu.org/onlinedocs/gcc/Gcov.html "gcov — a Test Coverage Program"
[LCOV]:  http://ltp.sourceforge.net/coverage/lcov.php "LCOV — the LTP GCOV extension"
