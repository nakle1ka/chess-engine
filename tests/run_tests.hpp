#pragma once
#include <vector>
#include <iostream>
#include <string>

using namespace std;

template <typename T>
struct TestCase
{
    T result;
    T expected;
};

template <typename T>
void RunTests(string name, vector<TestCase<T>> tests)
{
    size_t size = tests.size();
    int failed = 0;

    for (size_t i = 0; i < size; i++)
    {
        if (tests[i].expected != tests[i].result)
        {
            ++failed;

            cout << "\n[ FAIL ]\n";
            cout << "At " << name << ": " << i + 1 << " testcase\n";
            cout << "Expected: " << tests[i].expected << '\n';
            cout << "Found: " << tests[i].result << "\n";
        }
    }

    cout << "=== " << name << ": "
         << 100 - (failed * 100 / size)
         << "% TESTS PASSED ===" << endl;
};