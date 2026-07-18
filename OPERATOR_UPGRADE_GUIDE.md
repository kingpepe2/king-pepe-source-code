# KingPepe v31.1 Operator Upgrade Guide

## Supported Verified Path

This release was validated against an existing Windows datadir at:

`%APPDATA%\Kingpepe`

The verified wallet profile is SQLite descriptor wallet storage.

## Before Upgrading

1. Stop the existing KingPepe node cleanly.
2. Back up the whole datadir:
   - Windows: `%APPDATA%\Kingpepe`
   - macOS: `~/Library/Application Support/Kingpepe`
   - Linux: `~/.kingpepe`
3. Back up wallets separately.
4. Keep `kingpepe.conf` unchanged unless intentionally changing networking policy.

## Binary Replacement

Use the binary packages published with this release:

- Wallet GUI package: the Windows wallet release asset
- Daemon package: the daemon release asset

For GUI distribution, keep `kingpepe-qt.exe` in the same folder as the included DLLs and Qt plugin subfolders from the wallet release asset.

## First Start

Start normally with the existing datadir. Do not use `-reindex` unless you have a separate operational reason.

Recommended first checks:

```powershell
kingpepe-cli getblockchaininfo
kingpepe-cli getwalletinfo
kingpepe-cli getnetworkinfo
```

Expected compatibility signals:
- Chain remains `main`.
- Best height is not reset to zero.
- Wallet last processed block is at or near the chain tip.
- P2P subversion is `/Kingpepe:31.1.0/`.

## Rollback

If the node fails before writing new chain data, stop v31.1 and restore the full datadir backup before starting the old binary.

If the node has run and modified wallet or settings files, do not mix binaries against the modified datadir. Restore from backup first.

## Legacy Wallet Notice

Legacy Berkeley DB wallet migration was not verified in this run because no BDB KingPepe wallet sample was present. Operators with old legacy wallets should run a separate migration rehearsal on a copy before using production funds.
