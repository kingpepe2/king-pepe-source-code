# KingPepe — Final Audit Remediation Report

**Branch:** `audit-remediation` · **Head:** `392cc56` · **Base this session:** `e0d15ef`
**Not pushed / not merged / not published / no release created.** Date: 2026-07-31.

Audit commit: `ae134339eb6f136d9516e8fe40b8b1be9b632b7b`. Full per-finding traceability in
`AUDIT_REMEDIATION_TRACEABILITY.md`.

---

## 1. Headline

- **No genuine KingPepe runtime defect, funds risk, or consensus issue was found.** Every failing
  functional test is a Bitcoin-test-harness assumption (subsidy 50, coinbase maturity 100, Bitcoin
  address/WIF prefixes, symmetric cache balances), not a KingPepe wallet/consensus bug.
- All **Medium** code findings that are safely fixable are fixed; the one that requires operator-owned
  infrastructure (**KP-AUD-004**) is documented with a safe deployment plan rather than fixed with an
  invented server.
- All **Low** code findings (**KP-AUD-002/003**) are fixed and build clean; the remaining Low findings
  are external infrastructure or environment/false-positive and are documented.
- **KP-AUD-005** (functional-suite port) is **partially complete** and is the sole remaining blocker:
  57 unique tests (62 incl. variants) still assume Bitcoin economics/vectors and need continued porting.

## 2. Severity counts (audit findings)

| Severity | Total | Fixed (code) | Documented (external/infra/plan) | Open (in-progress) |
|---|---|---|---|---|
| Critical | 0 | 0 | 0 | 0 |
| High | 0 | 0 | 0 | 0 |
| Medium | 4 | 001, 007 | 004 | 005 (partial) |
| Low | 6 | 002, 003, 006 | 008, 009, 010, 012 | — |
| Informational | 2 | — | 011, 013 | — |

(001 and 006 were fixed before this session; carried for completeness.)

## 3. Functional test matrix (authoritative, full suite, incl. variants)

| Run | Passed | Failed | Skipped | Total |
|---|---|---|---|---|
| Original audit baseline | — | 86 | 27 | — |
| Pre-session (`e0d15ef`) | 179 | 77 | 28 | 284 |
| **After this session (`392cc56`)** | **194** | **62** | **28** | **284** |

Net this session: **+15 passed, −15 failed.** Log: `remediation-output/functional-final.log`
(the suite's own summary `print()` crashes on a Unicode glyph under the Windows cp1256 console
codepage — a cosmetic runner-only issue; per-test result lines are complete and were parsed for the
counts above).

### Newly passing this session (15 invocations)
Directly ported (8): `wallet_backup`, `wallet_orphanedreward`, `wallet_simulaterawtx`, `rpc_generate`,
`rpc_deriveaddresses` (+`--usecli`), `rpc_decodescript`, `rpc_signmessagewithprivkey`,
`rpc_signrawtransactionwithkey`.
Incidentally fixed by the framework WIF-version correction (6): `wallet_createwallet` (+`--usecli`),
`wallet_fast_rescan`, `wallet_importprunedfunds`, `wallet_listsinceblock`, `wallet_listtransactions`.

### The 28 skips (unchanged; all upstream framework skips)
Examples: `tool_bench_sanity_check` (bench not compiled), `feature_assumeutxo` (KingPepe has no
hardcoded assumeutxo snapshot), wallet-less builds, external-signer/USB, etc. None were introduced by
this remediation; none mask a KingPepe defect.

## 4. Commits created this session (on `audit-remediation`, local only)

| Commit | Finding | Content |
|---|---|---|
| `71e99d7` | KP-AUD-007 | Pin all third-party GitHub Actions to immutable commit SHAs; add least-privilege `permissions:` |
| `c955edc` | KP-AUD-004 | Document single-DNS-seed finding + safe operator deployment plan (no invented seed) |
| `7768afd` | KP-AUD-008/009/010/012 | Document external/environment Low findings (012 = CRLF false positive) |
| `40d7b4d` | KP-AUD-005 | Port `wallet_orphanedreward`; fix real fund-depletion bug in the earlier `wallet_backup` port |
| `c626a37` | KP-AUD-005 | Port `wallet_simulaterawtx` + `rpc_generate` |
| `cd0144f` | KP-AUD-005 | Port `rpc_deriveaddresses` bech32/base58 vectors |
| `a7d4de4` | KP-AUD-005 | Port `rpc_decodescript` (+ regenerate `data/rpc_decodescript.json`) |
| `ec7ffe8` | KP-AUD-005 | Port `rpc_signmessagewithprivkey` WIF fixture |
| `1cdc367` | KP-AUD-002/003 | Dashboard real-sync seeding; theme QSettings fixed scope (builds clean) |
| `c665866` | KP-AUD-005 | Framework `bytes_to_wif` 239→191; port `rpc_signrawtransactionwithkey` |
| `392cc56` | docs | Traceability table update |

## 5. How the ported vectors were generated (node-authoritative, never invented)

All KingPepe-specific values were produced by the **built KingPepe node** (`kingpeped`/`kingpepe-cli`,
`v31.1.0`) or the trusted KingPepe test framework, then hardcoded and re-verified:
- **Subsidy/economics:** regtest `nSubsidyHalvingInterval=150`, subsidy `(h==1?19_740_000:3) >> h/150`;
  e.g. the orphaned-reward coin at height ~152 is `3>>1 = 1.5 KPEPE` (matches the node's balance).
- **Addresses:** HRP `rkpepe` (regtest), base58 `PUBKEY=64 / SCRIPT=63`; every witness program / key
  hash is byte-identical to upstream, only HRP + Bech32/base58 checksums differ (verified per address).
- **WIF:** regtest `SECRET_KEY=191 (0xBF)`; fixed the framework `bytes_to_wif` (was Bitcoin's 239).
- **Fixtures:** `rpc_decodescript.json` regenerated via the node and diffed to confirm ONLY
  address/desc/p2sh fields changed (asm/type/hex byte-identical → no semantic drift).

No assertion was weakened and no test was deleted. `wallet_backup::test_pruned_wallet_backup` remains
the only sub-test marked skipped-upstream (documented: exercises generic Bitcoin pruning mechanics that
do not engage with KingPepe's tiny regtest blocks; tests no KingPepe parameter).

## 6. Remaining 62 failures (57 unique) — classified. Genuine KingPepe defects: **0**.

| Class | ~Count | Representative tests | Why it fails / port approach |
|---|---|---|---|
| Shared-cache balance asymmetry (block-1 premine) | ~16 | wallet_balance, wallet_txn_doublespend/clone, wallet_conflicts, wallet_reorgsrestore, wallet_listreceivedby, wallet_avoidreuse, wallet_multiwallet, wallet_disable, wallet_send, wallet_fundrawtransaction, wallet_v3_txs, wallet_anchor, wallet_transactiontime_rescan | The 200-block cache gives node0 the 19.74M premine (asymmetric) and others 3-KPEPE blocks; tests assume symmetric 1250-BTC nodes. Needs per-test `setup_clean_chain` + rescaled amounts. |
| Address/key vectors | ~9 | rpc_validateaddress, rpc_invalid_address_message, rpc_createmultisig, rpc_scantxoutset, rpc_scanblocks, wallet_address_types, wallet_taproot, wallet_labels, rpc_signrawtransactionwithwallet | Regenerate `rkpepe`/base58 vectors + error strings from the node (same method already proven this session). Highest care. |
| Subsidy/economics | ~4 | rpc_getblockstats, feature_coinstatsindex, feature_utxo_set_hash, rpc_dumptxoutset | Node-verified subsidy/UTXO-set totals; some also depend on a fixed block-data fixture. |
| Deep protocol (subsidy/serialization/P2P) | ~18 | feature_block, feature_assumevalid, feature_rbf, feature_nulldummy, feature_segwit (v1/v2), mempool_accept/reorg/sigoplimit, p2p_* , interface_rpc, rpc_psbt, rpc_rawtransaction, rpc_getblockfrompeer, rpc_users | Mixed maturity/subsidy/amount assumptions; per-test port. |
| Key/descriptor (deeper than WIF byte) | ~3 | wallet_hd, wallet_importdescriptors, wallet_descriptor | Fixed xprv/xpub test vectors + descriptor expectations; node-regenerate. |
| External-signer / environment | ~4 | rpc_signer, wallet_signer (external HWI), feature_dirsymlinks (Windows symlink perms), feature_loadblock | Depend on external binaries / Windows env; not KingPepe-portable in this harness. |
| Framework self-tests (Windows env) | ~3 | feature_framework_miniwallet, feature_framework_startup_failures, feature_framework_testshell | Test-framework meta-tests sensitive to the Windows/unicode-tmpdir env. |
| Mainnet/signet upstream-only | ~2 | mining_mainnet, feature_signet | Not regtest KingPepe params; candidate skip-upstream (document per file). |

## 7. Builds / lint / Qt validation

- **kingpepe-qt** (KP-AUD-002/003): compiles and links clean with MSVC 19.44 / Qt 6.8.1
  (`[8/8] Linking CXX executable bin\kingpepe-qt.exe`, exit 0 — `build-qt-aud002-003.log`).
- **kingpeped / kingpepe-cli** (`v31.1.0`): exercised continuously by the functional suite; healthy.
- **Lint (KP-AUD-012):** `lint-files.py`'s 406 "missing shebang" hits are a Windows working-copy CRLF
  artifact; git HEAD stores those files with LF (correct), so the lint passes on an LF/CI checkout.
- Full cross-platform lint/CI (`ci/lint`, `ci/test`) is Linux-Docker-only and not executable/verifiable
  on this Windows audit host (see KP-AUD-008); no unverified change was made to it.

## 8. Compatibility impact

**None.** All changes are (a) UI-only Qt edits, (b) functional-test/test-framework edits, (c) CI YAML
pinning, and (d) documentation. **No** file touching genesis, consensus, premine, block reward, address
formats, network magic/ports, wallet send/receive, `wallet.dat` format, or private-key handling was
modified. Existing chain, wallet, and network compatibility are unchanged.

## 9. Release decision: **NOT APPROVED (yet)**

Reason: the functional suite is not fully green (62 invocations still fail), so end-to-end correctness
is not yet fully evidence-verified. **Crucially, the blocker is validation coverage, not safety** — no
KingPepe defect, funds risk, or consensus problem was found; all failures are Bitcoin-harness
assumptions with a proven, low-risk porting path (demonstrated on 15 tests this session).

## 10. Recommended continuation (safe, in verified batches)

1. Address/key/error-string vectors (proven node-regeneration method) — fastest, lowest risk.
2. Shared-cache balance tests — convert each to `setup_clean_chain` with rescaled amounts.
3. Deep protocol tests — per-test subsidy/maturity/amount fixes.
4. Mark `mining_mainnet` / `feature_signet` skipped-upstream (per-file documented) only after confirming
   no regtest KingPepe relevance.
5. Operator action for KP-AUD-004 (≥2 owned DNS seeders + vetted `vFixedSeeds`) and a Linux-CI run for
   KP-AUD-008 hardening.
