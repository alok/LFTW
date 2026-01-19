# PRD: LFTW (Lean 4 FFTW Port)

## Goal
Port FFTW to Lean 4 using heavy metaprogramming to generate FFT kernels and planners. Target correctness against FFTW itself and aim for performance competitive with (or better than) OCaml FFT implementations on fixed-size transforms.

## Scope
- Implement a Lean 4 FFT core with generated kernels (codelets) and a planner.
- Use FFTW source as the reference/Oracle and test suite by downloading/building FFTW locally.
- Build a Lean-side test harness that compares Lean outputs against FFTW outputs for randomized inputs across sizes and radices.
- Establish reproducible performance benchmarks and track regressions.

## Non-Goals (for now)
- Full FFTW feature parity (threads, MPI, wisdom import/export, GPU backends).
- Formal proofs of FFT correctness/performance (allow `sorry` for now).

## Constraints
- Lean 4 implementation must remain computable unless a clear reason to use noncomputable defs.
- Prefer Lean metaprogramming (macros/elab) for code generation rather than external generators.
- No network dependency at runtime; FFTW is only used for local testing.

## Success Criteria
- `make check`-equivalent FFTW tests pass via a Lean-driven cross-check harness.
- Lean FFT matches FFTW outputs within numerical tolerances for randomized vectors across typical sizes.
- Benchmarks show Lean FFT performance competitive with or better than OCaml FFTs on at least some fixed-size cases.

## Proposed Architecture
1. **Core types & buffers**: complex number representation, strided buffers, in-place/out-of-place transforms.
2. **Kernel DSL**: Lean DSL to express FFTW-style codelets (radix-2/3/4/5/etc) with metaprogrammed expansion.
3. **Planner**: recursive decomposition, size factoring, kernel selection, and composition.
4. **Backend**: Lean-to-native compilation; optional C interop for SIMD intrinsics later.
5. **Testing**: FFTW build + runner to produce golden outputs for cross-checks.

## Milestones
- M0: Repo scaffolding + ralph loop + Linear work queue.
- M1: FFTW local build & test harness in `scripts/fftw/` and `Tests/`.
- M2: Minimal FFT (radix-2) in Lean + correctness tests vs FFTW.
- M3: Metaprogrammed codelet generator + multiple radices.
- M4: Planner + composition rules + buffer/stride handling.
- M5: Benchmarks + perf tuning; report vs OCaml baseline.

## Open Questions
- Which OCaml FFT library and benchmark sizes should we compare against?
- Do we need mathlib for complex numbers, or define a minimal complex type?
- SIMD strategy: pure Lean vs externs.
