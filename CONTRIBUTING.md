Contributing to KingPepe Core
=============================

KingPepe Core is maintained as a Bitcoin Core v31.1 based codebase with
KingPepe chain parameters, network identity, wallet paths, and product
branding.

Before opening a pull request:

1. Build the project locally.
2. Run the relevant unit, functional, or GUI tests for the area changed.
3. Explain the reason for the change and the tests performed.
4. Call out any consensus, wallet, network, mining, or RPC compatibility impact.

Consensus and network changes require extra care. Do not change chain
parameters, genesis data, subsidy rules, difficulty rules, message-start bytes,
address prefixes, default ports, checkpoints, chainwork, assume-valid values,
or wallet upgrade behavior unless the change is intentional, reviewed, and
documented.

## Build And Test

See [INSTALL.md](INSTALL.md) and the platform-specific files in [doc](doc) for
build instructions.

Common local checks:

```sh
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Functional tests live in [test/functional](test/functional). Unit tests live
under [src/test](src/test).

## Pull Request Guidelines

Keep changes focused and reviewable. Prefer small commits that can be built and
tested independently. Include a test plan in the pull request description.

Useful component prefixes include:

- `consensus`
- `wallet`
- `net` or `p2p`
- `rpc`
- `qt` or `gui`
- `mining`
- `build`
- `doc`
- `test`

Avoid unrelated formatting churn in behavioral changes. If formatting-only
cleanup is needed, keep it separate.

## Translations

Translation files are maintained separately from normal source changes. Avoid
large manual translation edits unless the translation workflow for this project
requires them.
