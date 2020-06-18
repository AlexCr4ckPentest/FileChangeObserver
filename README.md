# FileChangeObserver

## Dependencies
1. C++ Compiler (with C++17 support)
1. CMake (>= 3.1)
1. Boost library (>= 1.67.0)

## Building
1. `cd FileChangeObserver`
1. `mkdir build && cd build`
1. `cmake .. && make`
1. Run `./file-change-observer -h` for show help

## Using
1. Select monitoring directory (directories)
1. Specify paths through the `-P` option: `./file-change-observer -P <paths>`
1. Specify sleep timeout through the `-T` option: `./file-change-observer -T <timeout>`
1. Specify update iteration through the `-U` option: `./file-change-observer -U <update_iteration>`
### Example
`./file-change-observer -T 500 -U 10 -P /home/user /usr/bin /usr/etc`
This command start monitoring files in /home/user /usr/bin /usr/etc directories with sleep timeout=500ms, update iteration=20
