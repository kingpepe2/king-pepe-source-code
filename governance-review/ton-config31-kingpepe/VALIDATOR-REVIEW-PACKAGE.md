# KingPepe TON Config 31 validator review package

Generated: 2026-08-31T11:34:14.009Z

Status: **NO-GO / public technical review only / no transaction sent**. No query ID, expiration, final submission BOC, signature, vote, or broadcast was generated in this phase. Existing BOCs in this directory are review fixtures; the final registration envelope must be regenerated from live state only after genuine validator readiness and separate authorization.

## Project and governance request

KingPepe ↔ TON is preparing a 1:1 reserve-backed bridge in which native KPEPE remains on the KingPepe chain and TON issuance is intended to remain bounded by verified native reserves. The project’s seven signer identities are not TON validators. Production execution, minting, binding, and payouts remain disabled.

The sole governance request is to add `-1:a6e0023cfde6efd21c8de273b9a5fd67b07fd2c513f312504b44d6ff7c906108` to TON Config 31. Nothing is removed and no configuration parameter other than 31 changes.

- Proposal-cell ID: `E8131C85497C998C8FCFDEE823637C32EBB606DF73BE7CC9653F1BC11B7229E9` (decimal `104970347807858262948617335134309539262605470897601623395341607553733904968169`)
- Official Config contract: `-1:5555555555555555555555555555555555555555555555555555555555555555`
- Before hash: `4E65911BAF281EC692281255642BE840A938D6DB2162DF266ADF7806337D3FBE`
- After hash: `F9B2410C47BDB8C2167F005C905F814F2B4593E143B533978ACD48839D4677FD`
- Unchanged existing entries: 9
- Added entry: `-1:a6e0023cfde6efd21c8de273b9a5fd67b07fd2c513f312504b44d6ff7c906108`
- Removed entries: none
- Changed parameter indices: `[31]`

`CONFIG31-DIFF.json` and the independent verifier prove the exact entry-set relationship and reject an extra address, removal, parameter substitution, critical flag, stale current-hash guard, or trailing field/reference.

## Official counter order and getter warning

Pinned official TON source commit: `3d478cbde854be03a18ab2a59f8fc3c565cf7d14`.

- `crypto/block/block.tlb:652` defines `rounds_remaining:uint8 wins:uint8 losses:uint8`.
- `crypto/smartcont/config-code.fc:229-251` loads and stores those counters in the same order.
- Regression bytes `03 00 03` therefore mean `rounds_remaining=3, wins=0, losses=3`. Interpreting them as three wins and zero losses is invalid.
- The deployed `get_proposal` getter explicitly comments that wins/losses are mixed up and returns `rounds_remaining, losses, wins` at `config-code.fc:610-614`.

Governance counters must therefore be decoded from finalized raw Config persistent account-data BOCs. TonCenter, TonAPI, and getters may transport raw data but must not supply interpreted counters.

Official source links:

- https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/crypto/block/block.tlb
- https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/crypto/smartcont/config-code.fc
- https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/crypto/smartcont/config-proposal-vote-req.fif
- https://github.com/ton-blockchain/ton/blob/3d478cbde854be03a18ab2a59f8fc3c565cf7d14/doc/ConfigParam-HOWTO

## Fresh Config 11 and Config 34 requirements

The raw finalized snapshot at Masterchain block 89690133 (2026-08-31T11:28:03.000Z) records Config 11 hash `F88D65DDDE51ACD9C3EF1926EC5EFA1AAFCA63F30B6CEC48DBA7507A89111809`. Normal proposals require 2..6 rounds, at least 2 wins, no more than 5 losses, and storage lifetime 1,000,000..10,000,000 seconds. Storage pricing is 1 nanoton per bit-second and 500 nanotons per cell-second.

Freshly verified Config 34 hash: `651B36DB5847DC71123B8901EE04447D10DFA5CDE844B84655FA80FC6A712297`; valid until 2026-09-01T04:05:28.000Z; total weight `1152921504606846796`. Official contract code starts each round at `floor(3*total_weight/4)` and wins only on strict sign crossing. Required current yes weight is therefore `864691128455135098`, strictly greater than 75%. Validator count is not a substitute for validator weight.

Config 11 requires two winning rounds. Config 36 was absent, so this package has no available next-set identity/weight evidence and counts no future-round support.

## Same-ID voteability

The proposal-cell ID hashes the unchanged `cfg_proposal` intent, so a fresh registration of the exact same change has the same ID. Official `config-code.fc` deletes expired, failed, and accepted dictionary entries and creates a proposal whenever the hash is absent from the current live dictionary. Official `config-proposal-vote-req.fif` accepts the supplied 256-bit hash, and validator-engine `createproposalvote` passes it through without a historical-ID cache. There is no permanent suppression list. A future occurrence is fresh state; the prior rejected occurrence remains rejected historical evidence.

## Independent decode and verification

```powershell
Get-FileHash production/ton-governance-proposal-v2/config31-before.boc -Algorithm SHA256
Get-FileHash production/ton-governance-proposal-v2/config31-after.boc -Algorithm SHA256
Get-FileHash production/ton-governance-proposal-v2/proposal-cell.boc -Algorithm SHA256
node --import tsx scripts/phase-c/verify-ton-config31-proposal-package.ts
node --import tsx --test test/unit/ton-config31-proposal-package.test.ts test/unit/ton-config31-proposal-v2-artifacts.test.ts test/unit/config31-quorum-submission.test.ts test/unit/validator-support-ledger.test.ts
rg -n "rounds_remaining|wins|losses" .upstream/ton-v2026.08/crypto/block/block.tlb .upstream/ton-v2026.08/crypto/smartcont/config-code.fc
```

To inspect live governance state, export the official Config contract account-data BOC with a pinned official lite-client at a finalized Masterchain block, then run the project raw-data decoder. Do not infer acceptance from proposal disappearance; require the candidate to appear in live Config 31.

## Voting instructions after a separately authorized registration

First verify in raw finalized account data that one active occurrence exists with ID `E8131C85497C998C8FCFDEE823637C32EBB606DF73BE7CC9653F1BC11B7229E9`, the exact proposal cell, fresh expiration, zero wins/losses, and the current validator-set ID. Then use official validator tooling and your own custody procedure. The official automated command is:

```text
createproposalvote 104970347807858262948617335134309539262605470897601623395341607553733904968169 vote-msg-body.boc
```

The generated body must be delivered from the validator’s normal controlling Masterchain wallet to the official Config contract. Independently decode the vote request (`vote#566f7445 validator_index:uint16 proposal_hash:uint256`) before signing. Never share validator keys, never use project signer keys, and never vote against an absent or semantically different occurrence.

## Official public coordination-channel provenance

Authoritative TON node documentation identifies `https://t.me/tonstatus` for Mainnet validator action notices, `https://t.me/validators` / `https://validators.ton.org` for validator-owner operations, and official TON Foundation announcements at `https://t.me/tonblockchain`. Source pages: `https://github.com/ton-blockchain/docs/blob/main/content/nodes/status.mdx` and `https://github.com/ton-blockchain/docs/blob/main/content/nodes/cpp/run-validator.mdx`. The durable `OUTREACH-LOG.json` records which verified surfaces were actually usable and contacted; no private contacts were scraped.

## Security and risk disclosures

- Config 31 grants special-contract treatment; validators should audit the pinned candidate code, StateInit, 4-of-7 authorization, and operational risks independently.
- The same proposal-cell ID does not mean the rejected occurrence was revived.
- Technical package validity does not establish validator support or guarantee approval.
- KingPepe does not request validator signing secrets, wallet access, funds, private contact details, or remote commands.
- Captured Config 34 evidence expires with its validator set and must be refreshed before action.
- The review envelope in this package is not the future final submission envelope.
- Submission would not equal acceptance or special-status verification.
- HARD_STOP remains true; bridge, mint, binding, canary, payout, deployment, and production activation remain blocked.
