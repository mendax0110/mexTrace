# mexTrace - Process Stack Trace Utility

![License](https://img.shields.io/badge/license-MIT-blue.svg)

mexTrace is a command-line utility for capturing and displaying stack traces of running processes on Unix-like systems. It provides process inspection capabilities and detailed call stack information.

## Features

- List all running processes with their PIDs and names
- Capture stack traces of specific processes
- Display detailed function call information with addresses
- Support for verbose debugging output
- Color-coded console output for better readability

## Installation

### Prerequisites
- C++23 compatible compiler
- CMake (version 3.25 or higher)
- Unix-like operating system (Linux)

### Build Instructions
```bash
git clone https://github.com/mendax0110/mexTrace.git
```

```bash
cd mexTrace
```

```bash
mkdir build && cd build
```

```bash
cmake ..
```

```bash
cmake --build .
```

### Note
Targets must be build with '-fno-omit-frame-pointer -g' flags, otherwise mexTrace cannot diplay all infos.