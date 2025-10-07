# LSM Tree in C++

Simple Log-Structured Merge tree with custom SkipList implementation in C++17.

## Build & Test

```bash
mkdir build && cd build
cmake .. && make
./lsm_storage_test  # or: ctest
```

## Features

- Custom **SkipList** data structure
- **Multi-memtable** LSM storage with automatic freezing
- **Thread-safe** operations with proper locking
- **Comprehensive tests** (24 tests across 3 suites)
