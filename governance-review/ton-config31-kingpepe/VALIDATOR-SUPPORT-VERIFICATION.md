# Validator support evidence verification

Generated: 2026-08-31T07:13:27.980Z

Status: **NO-GO**. The ledger contains the complete Config 34 validator set, freshly matched to finalized Masterchain block 89651134 captured at 2026-08-31T07:03:41.000Z, and no claimed support.

- Config 34 hash: `F1C18570823EF34633918E0D823EC0EF51D24F0ABCC48CA519C92DA8BAFC2FAA`
- Set validity: 2026-08-30T15:40:56.000Z through 2026-08-31T09:53:12.000Z
- Validator identities: 383 total, 100 main validators
- Total weight: `1152921504606846801`
- Strict threshold floor: `864691128455135100`
- Required yes weight: `864691128455135101`
- Required winning rounds: 2
- Attributable verified support entries: 0
- Verified off-chain support-indication weight: `0`
- Actual on-chain vote weight: `0` (no active occurrence)
- Winning rounds covered: 0
- Config 36: absent; future-round evidence is `UNAVAILABLE_NOT_COUNTED`

## Counting rules

The calculator binds every entry to its Config 34 index, Ed25519 public key, ADNL identity, and exact weight. It rejects duplicate indices/keys, changed weights, stale Config 34 hashes or validity windows, stale evidence, duplicate evidence IDs, anonymous sources, non-HTTPS sources, invalid signatures, and any attempt to count an entry without evidence.

A verified off-chain indication remains an off-chain indication; it is never labeled or counted as an on-chain vote. Informal interest, anonymous messages, duplicated identities, and unverified claims count as zero. Actual votes can only be observed after registration by decoding finalized raw Config account-data BOCs. Operators must not use validator consensus keys outside their own approved security procedures merely to populate this ledger.

## Reproduce

```powershell
node --import tsx --test test/unit/validator-support-ledger.test.ts
node --import tsx scripts/phase-c/prepare-validator-coordination-package.ts
```
