# Various test vectors

## mainnet_alt.json

This file contains deterministic block timestamps and nonces for
`mining_mainnet.py`. The fixture is an alternate KingPepe mainnet chain used
only by the functional test runner to exercise difficulty adjustment logic that
regtest does not cover.

KingPepe mainnet uses:

- genesis hash
  `00000a00a75c7ed12c71b9a8b73c01576009d62a0a606c0a1ef37b043c520fb2`
- initial bits `0x1e0ffff0`
- retarget interval 120 blocks
- target spacing 60 seconds

The fixture mines blocks 1 through 120 from the KingPepe mainnet genesis. Blocks
1 through 119 use `0x1e0ffff0`; block 120 uses the maximum permitted difficulty
increase, `0x1e03fffc`. Timestamps are spaced 15 seconds apart, forcing the
first retarget period to the minimum quarter-timespan allowed by the existing
KingPepe proof-of-work rules.

The coinbase output script is a fixed P2PKH scriptPubKey used only to make the
serialized coinbase transaction deterministic. It is not a payout destination
used by production code.

Regenerate the file from a clean checkout with the current Python functional
test framework helpers if KingPepe mainnet consensus parameters intentionally
change. Do not copy Bitcoin mainnet vectors into this file.
