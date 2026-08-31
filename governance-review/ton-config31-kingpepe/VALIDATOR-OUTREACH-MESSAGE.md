# Ready-to-send English validator review message

> Subject: Pre-submission technical review request - add KingPepe Teleport candidate to TON Config 31
>
> We invite TON validator operators to independently review a proposed add-only Config 31 change for the KingPepe <-> TON Teleport project. This is a pre-submission technical review request. The project is preparing a 1:1 reserve-backed bridge; its project signer network is not represented as TON validator governance. Mainnet bridge execution remains disabled.
>
> Requested addition: `-1:a6e0023cfde6efd21c8de273b9a5fd67b07fd2c513f312504b44d6ff7c906108`
> Proposal-cell ID: `E8131C85497C998C8FCFDEE823637C32EBB606DF73BE7CC9653F1BC11B7229E9`
> Config 31 before hash: `4E65911BAF281EC692281255642BE840A938D6DB2162DF266ADF7806337D3FBE`
> Config 31 after hash: `F9B2410C47BDB8C2167F005C905F814F2B4593E143B533978ACD48839D4677FD`
> Removed entries: none. Changed configuration parameters: 31 only.
>
> Please reproduce the Config 31 cell diff and proposal-cell decode from the public-safe review package. The official persistent status layout is `rounds_remaining -> wins -> losses`. The deployed `get_proposal` getter returns the final two counters in swapped display order, so please use raw Config contract account-data BOCs for governance counters.
>
> At finalized Masterchain block 89651134, the current Config 34 set has total weight `1152921504606846801`; the strict >75% minimum is `864691128455135101` by validator weight, not validator count. Config 11 requires two winning rounds. Config 36 was absent, so future-round support is unavailable and not counted.
>
> No new proposal occurrence is currently active on-chain, so no validator vote can yet be cast. We request independent verification and an attributable public indication of whether your validator expects to support the exact proposal if and when a fresh occurrence becomes active. An off-chain response is not an on-chain vote and does not guarantee acceptance.
>
> Validators must independently verify the proposal with official TON tooling and follow their own security and key-custody procedures. KingPepe does not request signing secrets, wallet access, funds, private contact details, or remote commands.
>
> Security note: the previous occurrence with this deterministic proposal-cell ID was rejected and remains immutable historical evidence. A future registration would be a fresh occurrence; it would not revive or reinterpret the rejected one. Submission is not acceptance, and bridge activation remains hard-stopped until Config 31 activation and independent special-status verification.

Publication and response status are recorded separately in `OUTREACH-LOG.json`.
