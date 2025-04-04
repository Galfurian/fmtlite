# fmtlite

**fmtlite** is a compact, freestanding implementation of `vsnprintf` for low-level C environments, such as kernels, embedded systems, and operating systems. It provides safe, flexible formatting without relying on any standard C library.

---

## Features

- Fully freestanding — no `libc` required
- Implements:
  - `vsnprintf`
  - `vsprintf`
- Supports the following format specifiers:
  - `%d`, `%i` — signed integers
  - `%u`, `%x`, `%X`, `%o` — unsigned integers (base 10, 16, 8)
  - `%c`, `%s`, `%%` — character, string, literal percent
  - `%p` — pointer (hex)
  - `%f`, `%F` — float with precision
  - `%n` — write output length
- Handles:
  - Field width, precision
  - Flags: `-`, `+`, `0`, space
  - Length modifiers: `h`, `hh`, `l`, `ll`
  - Optional: supports `%lf` and `%ld`
- Supports `NULL` buffer and `size == 0` to compute output length safely

---

## Requirements

`fmtlite` depends only on the following standard headers:

```c
#include <stdarg.h>  // for va_list, va_start, va_end, va_arg
#include <stddef.h>  // for size_t
```

## Usage

```bash
git clone https://github.com/Galfurian/fmtlite
```

Then:

```c++
#include <fmtlite/fmtlite.h>

char buf[128];
vsnprintf(buf, sizeof(buf), "Value: %d", 10);
```

## Build & run tests

```bash
mkdir build && cd build
cmake ..
make
ctest
```

## License

MIT License — see `LICENSE.md` for details.
