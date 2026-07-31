# KP-AUD-004 — DNS seed resilience: finding & safe deployment plan

**Status:** Open — **documented, no code change** (correctly requires operator-verified infrastructure).
**File:** `src/kernel/chainparams.cpp:114-115` (mainnet `CMainParams`). **Do NOT change consensus,
genesis, ports, network magic, address prefixes, or chain identity.**

## Current state (verified read-only, 2026-07-31)
- Mainnet `vSeeds` contains exactly one entry: `node3.walletbuilders.com`.
- Mainnet `vFixedSeeds` is **empty** (`vFixedSeeds.clear();`).
- DNS resolution of `node3.walletbuilders.com` returns:
  - `2a01:7c8:bb0d:43c:5054:ff:fed2:a8f6` (public IPv6, Hetzner range) — usable.
  - `fd74:330f:f903::1` (**RFC 4193 private ULA**) — **not a usable public peer** (config smell).
- The live mainnet node is connected to a real KingPepe peer `85.10.148.5:24028`
  (`subver /Kingpepe:25.0.0/`), proving reachable nodes exist — but **ownership/operator of that IP
  is not documented or verifiable by this audit.**

## Why no code change was made
Per the remediation constraints, a second seed may be added **only** if it is live, controlled,
publicly reachable, returns valid KingPepe peers, **and has documented ownership**. No such
independently-operated second DNS seed is known/verifiable, and hard-coding an arbitrary peer IP
(e.g. `85.10.148.5`) as a fixed seed **without the operator's consent and documented ownership** would
be "inventing an unverified server" — explicitly disallowed. Adding seeds also changes bootstrap
**network behavior**, a designated stop-point. Therefore this finding is documented, not code-changed.

## Risk (unchanged)
A clean install with no `peers.dat`/`addnode` depends on a **single third-party DNS seed**. If it is
down, withdrawn, censored, or serves adversarial peers, fresh-node bootstrap is fragile
(availability + eclipse-surface). Existing nodes with a populated `peers.dat` are unaffected.

## Safe deployment plan (operator actions — no consensus/port/magic/prefix change)
1. **Stand up ≥2 independent DNS seeders** on distinct networks/operators, each running the KingPepe
   crawler (or a maintained `bitcoin-seeder` fork) and returning only reachable `/Kingpepe/` nodes on
   port 24028. Record operator, host, and contact for each (documented ownership).
2. **Fix the ULA leak:** ensure the seeder for `node3.walletbuilders.com` does not return RFC 4193
   (`fd00::/8`) / RFC 1918 addresses.
3. **Generate a vetted fixed-seed set** from a snapshot of long-lived, reachable mainnet nodes
   (`contrib/seeds/` tooling), and populate `vFixedSeeds` for `CMainParams` **only** from that vetted
   list, in a coordinated release. Do not touch testnet/regtest/signet seed blocks.
4. **Add the new DNS seeds** to `vSeeds` (append; keep the existing one) once each is verified live and
   ownership-documented.
5. **Regression:** clean-datadir bootstrap smoke test (empty `peers.dat`, no `addnode`) must discover
   peers via DNS and via the fixed-seed fallback with DNS blocked.
6. Ship the seed additions in a **UI/config-safe maintenance release** (e.g. v1.1.x). Consensus, ports
   (24028), magic (`f3eace21`), prefixes (51/50/178), and bech32 HRP (`kpepe`) remain unchanged.

## Decision required from the project owner
Provide the hostnames + documented ownership of the additional DNS seeders (and/or approve a vetted
fixed-seed list). Once supplied and verified live, the `vSeeds`/`vFixedSeeds` change is a
one-line-per-entry, consensus-safe edit that can be made and tested.
