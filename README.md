# HF-UTILITIES

This repository contains a collection of C/C++ utility classes and helpers that are
hardware agnostic.  The code has been trimmed of RTOS, mutex, and console
dependencies so it builds cleanly with nothing more than the C++17 standard
library.

## Building

The project does not include a dedicated build system.  The utilities can be
compiled with any C++17 compatible compiler by adding the `include` directory to
 your include path.  For example:

```bash
g++ -std=c++17 -Iinclude -c src/Utility.cpp
```

## Provided Utilities

- Mathematical helpers (linear mapping, conversions, etc.)
- Ring buffers and dynamic arrays
- Timestamped variables
- Simple and advanced state machines
- CRC calculation routines

These utilities are self-contained and avoid any direct console output.  They
only use the C++ standard library and do not depend on platform specific
headers.  The legacy `LeastSquaresFit` helper was removed to reduce
dependencies.

A small helper in `Utility.h` provides a basic millisecond timer so the
utilities can run without any external threading support or mutex primitives.
