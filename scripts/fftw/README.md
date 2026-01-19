# FFTW Oracle Harness

This directory contains a deterministic FFTW build script and a small oracle
binary for cross-checking Lean outputs.

## Build

```sh
scripts/fftw/build.sh
```

Artifacts land in `.deps/fftw/`:

- `.deps/fftw/install` (FFTW install prefix)
- `.deps/fftw/bin/fftw-oracle` (oracle CLI)

## Oracle usage

Input format: `N` lines of `real imag` (double precision).

```sh
.deps/fftw/bin/fftw-oracle --size 8 --input input.txt --output output.txt
```

Use `--inverse` for inverse transforms (note: FFTW does not scale by `1/N`).
