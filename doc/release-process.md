KingPepe Core Release Process
=============================

This checklist is for KingPepe Core maintainers preparing a public release.

## Before A Release Candidate

1. Update the client version in [CMakeLists.txt](../CMakeLists.txt).
2. Rebuild all release binaries from a clean source checkout.
3. Regenerate public man pages if user-facing command-line options changed.
4. Run unit, regression, integration, and relevant functional tests.
5. Verify chain continuity against a copy of an existing KingPepe datadir.
6. Verify wallet startup and migration behavior on copied wallet data.
7. Generate release notes and binary SHA256 hashes.

## Consensus Review

Before tagging, explicitly verify that the release preserves:

- genesis block and genesis hash
- premine rules
- block subsidy and halving interval
- difficulty and block timing rules
- checkpoints, minimum chainwork, and assume-valid values
- address prefixes and Bech32 HRP
- message-start bytes and default network ports

Any intended change in these areas must be documented and reviewed as a
consensus or network-identity change.

## Binary Packaging

Windows GUI packages must include `kingpepe-qt.exe`, `kingpepe-cli.exe`,
`kingpepe-tx.exe`, all required DLLs, and Qt plugin subdirectories required for
startup on a clean Windows installation.

Daemon packages must include `kingpeped.exe` and any runtime dependencies
required by the daemon.

Do not include source build directories, vcpkg installation trees, test
executables, logs, datadirs, wallet files, private keys, or temporary files in
release packages.

## Tagging And Publishing

1. Commit the final source tree.
2. Create a signed tag for the release version.
3. Upload binary assets and SHA256 hashes to GitHub Releases.
4. Verify the published assets by downloading them into a clean folder and
   checking hashes and executable versions.
5. Announce the release only after the published assets have been verified.
