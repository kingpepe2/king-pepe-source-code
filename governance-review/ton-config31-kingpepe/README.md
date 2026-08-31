# KingPepe TON Config 31 pre-submission review

This directory is the public-safe technical review package for a possible fresh TON Config 31 proposal occurrence. It is not a submission package.

Current status: **NO-GO / HARD_STOP enabled / no transaction sent**.

- No new proposal occurrence is active on-chain.
- No validator vote can yet be cast.
- The previous occurrence is conclusively `REJECTED` and is not revived or reclassified.
- No query ID, expiration, final submission BOC, wallet access, signature, vote, or broadcast was generated for this outreach phase.

## Exact request

Add this one Masterchain address to Config 31:

`-1:a6e0023cfde6efd21c8de273b9a5fd67b07fd2c513f312504b44d6ff7c906108`

- Proposal-cell ID: `E8131C85497C998C8FCFDEE823637C32EBB606DF73BE7CC9653F1BC11B7229E9`
- Config 31 before: `4E65911BAF281EC692281255642BE840A938D6DB2162DF266ADF7806337D3FBE`
- Config 31 after: `F9B2410C47BDB8C2167F005C905F814F2B4593E143B533978ACD48839D4677FD`
- Added entries: the candidate above, exactly once
- Removed entries: none
- Changed configuration parameters: `[31]` only

See `VALIDATOR-REVIEW-PACKAGE.md` for the rationale, official source references, same-ID voteability analysis, voting requirements, and security disclosures. See `VALIDATOR-OUTREACH-MESSAGE.md` for the exact public review request.

## Verify the published package

Use Node.js 20 or newer in this directory:

```bash
npm install --ignore-scripts
npm run verify
```

The verifier independently decodes the proposal cell, Config 31 before/after cells, Config 11, `outreach-current-validator-set-config34.boc`, and the finalized raw Config contract account-data BOC. The compatibility file `current-validator-set-config34.boc` contains the same current Config 34 cell. It rejects a changed candidate, removal, extra Config 31 entry, parameter other than 31, missing current-hash guard, mismatched proposal ID, active same-ID occurrence, mismatched validator identity/weight, counted unverifiable support, present Config 36 claim, or altered safety state.

Verify file bytes independently:

```powershell
Get-FileHash *.boc -Algorithm SHA256
Get-FileHash *.json -Algorithm SHA256
Get-FileHash *.md -Algorithm SHA256
```

Compare the results with `SHA256SUMS.txt`.

## Refresh the raw live state

Use a release you independently built or verified from the pinned official TON source. With the official Mainnet global configuration:

```text
lite-client -C global.config.json -c "saveaccountdata config-account-data-live.boc -1:5555555555555555555555555555555555555555555555555555555555555555"
```

Decode the persistent account-data BOC directly. Do not use a getter, TonCenter, or TonAPI to interpret proposal counters. The official TL-B order is:

`rounds_remaining -> wins -> losses`

Regression bytes `03 00 03` mean `rounds_remaining=3`, `wins=0`, `losses=3`. Any interpretation as three wins and zero losses is invalid. The deployed getter swaps the displayed wins/losses fields.

Pinned official source:

- [`block.tlb`](https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/crypto/block/block.tlb)
- [`config-code.fc`](https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/crypto/smartcont/config-code.fc)
- [`config-proposal-vote-req.fif`](https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/crypto/smartcont/config-proposal-vote-req.fif)
- [`ConfigParam-HOWTO`](https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/doc/ConfigParam-HOWTO)

## Current weighted threshold

The raw finalized snapshot at Masterchain block `89690133` has:

- Config 34 hash: `651B36DB5847DC71123B8901EE04447D10DFA5CDE844B84655FA80FC6A712297`
- Set valid until: `2026-09-01T04:05:28.000Z`
- Total validator weight: `1152921504606846796`
- Strict required yes weight: `864691128455135098`
- Required winning rounds: `2`
- Config 36: absent; future-round evidence is unavailable and not counted
- Verified attributable off-chain support weight: `0`
- Actual on-chain vote weight: `0`

Threshold calculations use validator weight, never validator count. An off-chain response is not an on-chain vote and does not guarantee acceptance.

## Security

Validators must independently verify this package and follow their own security and key-custody procedures. KingPepe does not request signing secrets, wallet access, funds, private contact details, or remote commands.
