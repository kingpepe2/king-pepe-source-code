KingPepe Core v31.1.0 Release Notes
===================================

KingPepe Core v31.1.0 upgrades the KingPepe codebase to the Bitcoin Core v31.1
engine while preserving KingPepe chain identity and compatibility.

## Upgrade Notes

Before upgrading, stop the running KingPepe node cleanly and back up the full
KingPepe datadir and wallet files.

Default datadir locations remain unchanged:

- Windows: `%APPDATA%\Kingpepe`
- macOS: `~/Library/Application Support/Kingpepe`
- Linux: `~/.kingpepe`

Start the new binary with the existing datadir. Do not use `-reindex` unless
you have a separate operational reason.

## Compatibility

This release is intended to continue from the existing KingPepe chain without a
chain reset. It preserves KingPepe genesis data, premine behavior, subsidy
rules, halving interval, difficulty rules, block timing, address formats,
message-start bytes, ports, checkpoints, chainwork, assume-valid behavior,
wallet paths, config names, and product identity.

## Binaries

Release binaries are distributed through GitHub Releases. Windows GUI packages
must keep `kingpepe-qt.exe` beside the included DLLs and Qt plugin folders.

## Verification

Operators should verify the published SHA256 hashes before replacing binaries.
After first start, confirm:

```sh
kingpepe-cli getblockchaininfo
kingpepe-cli getnetworkinfo
kingpepe-cli getwalletinfo
```

Expected signals include chain `main`, the previous chain height preserved, and
subversion `/Kingpepe:31.1.0/`.
