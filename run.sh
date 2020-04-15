#!/bin/sh

run_unit_tests() {
    ret=0
    found_tests=0

    for test_bin_path in $(find bin | grep test); do
        found_tests=1
        echo 'Running '${test_bin_path}
        ./${test_bin_path}
        if [ $? = 1 ]
        then
            ret = 1
        fi
        echo '==========================================================================='
    done

    if [ ${ret} = 0 ] && [ ${found_tests} = 1 ]
    then
        echo 'All tests ran'
    fi
    if [ ${ret} = 1 ] && [ ${found_tests} = 1 ]
    then
        echo 'One or more tests failed to run'
    fi
}

run_unit_tests > results.txt;
grep 'FAIL' results.txt

if [ $? = 0 ]
then
    echo 'One or more tests failed' >> results.txt
    echo 'One or more tests failed'
    exit 1
fi
echo 'All tests passed' >> results.txt
echo 'All tests passed'
