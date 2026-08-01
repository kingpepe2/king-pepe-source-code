# KingPepe Core — Security Audit & Remediation Summary (Public)

This is a **public, high-level** summary of the security audit and its remediation for
KingPepe Core v31.1.0. Detailed internal traceability, raw command logs, and per-finding
evidence are retained privately by the maintainers and are **not** part of the published
source tree.

## Scope & commits

| | |
|---|---|
| Audit baseline commit | `ae134339eb6f136d9516e8fe40b8b1be9b632b7b` |
| Release-candidate commit | `9735c2f87b4c3af2d53e34fc204531a98b238cc1` |
| Version | v31.1.0 |

The release-candidate commit `9735c2f` is a **verified superset** of the earlier internal
remediation head recorded during the audit (`c665866…`): it contains every remediation change
from that head **plus** two verified CI corrections (a macOS compile fix and a Windows
test-timeout bound) and the official installer branding. No historical remediation evidence was
altered; the detailed record is preserved in the maintainers' private audit archive.

## Findings status

Severity counts — Critical: 0 open · High: 0 open · Medium: 1 open · Low: 3 open.

| ID | Severity | Status | Summary |
|---|---|---|---|
| KP-AUD-001 | Medium | Resolved | GUI kept visual-only; no unapproved wallet-lock timer. |
| KP-AUD-004 | Medium | **Open (operational)** | Only one DNS seed ships; a second independently-operated seed is an infrastructure task. |
| KP-AUD-005 | Medium | In progress | KingPepe-specific functional-test vectors ported and individually verified; a subset remains classified. |
| KP-AUD-007 | Medium | Resolved | CI third-party actions pinned to immutable SHAs; least-privilege workflow permissions. |
| KP-AUD-002 | Low | Resolved | Dashboard shows real sync state instead of a hard-coded "Synced". |
| KP-AUD-003 | Low | Resolved | Theme setting persists through one consistent UI settings scope. |
| KP-AUD-006 | Low | Resolved | CI runner labels corrected to supported hosted runners. |
| KP-AUD-008 | Low | **Open (Linux-verify)** | Linux CI install scripts to gain checksum/commit pinning; verifiable only on a Linux runner. |
| KP-AUD-009 | Low | **Open (external)** | Website should enforce an HTTP→HTTPS redirect (CDN/edge configuration). |
| KP-AUD-010 | Low | **Open (external)** | Public API description lists a localhost server entry; fix lives in the explorer service, not this repo. |
| KP-AUD-011 | Informational | Open | Internal Bitcoin→KingPepe string branding, compatibility-safe only. |
| KP-AUD-012 | Low | No defect | Lint "missing shebang" is a Windows CRLF working-copy artifact; passes on an LF/CI checkout. |
| KP-AUD-013 | Informational | Environment | Reproducible vcpkg manifest builds need a full pinned vcpkg checkout. |

## Release gate

The four **open external/operational** items — **KP-AUD-004, 008, 009, 010** — are not
source-code defects in this repository and cannot be closed from within the tree. Until they
are resolved and required CI runs green on a published branch, the release is considered
**NOT approved for publication**.

## Protected network parameters (unchanged by remediation)

Mainnet P2P port `24028` · message magic `f3 ea ce 21` · Bech32 HRP `kpepe` (regtest `rkpepe`) ·
coinbase maturity `20` · block-1 premine `19,740,000 KPEPE` · normal block subsidy `3 KPEPE` ·
existing genesis and chain compatibility. No consensus, premine, subsidy, address-format, or
wallet-compatibility value was changed.
