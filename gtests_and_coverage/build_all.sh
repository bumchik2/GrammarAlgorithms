mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
cd ..
./install_deps.sh python3.6
# python version should be the same as the one in your system
cd build
make -j2
make coverage_report
# !for some reason this script works just fine only after being run twice!

# after this we have coverage reports in 'build' directory,
# to rebuild coverage report simply run make coverage_report again
