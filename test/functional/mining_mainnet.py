#!/usr/bin/env python3
# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test mining on an alternate KingPepe mainnet

Test mining related RPCs that involve difficulty adjustment, which
regtest doesn't have.

It uses an alternate KingPepe mainnet chain. See data/README.md for how it was
generated.

Mine one KingPepe retarget period worth of blocks with a short interval in
order to maximally raise the difficulty. Verify this using the getmininginfo RPC.

"""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_approx,
    assert_equal,
)
from test_framework.blocktools import (
    create_coinbase,
    nbits_str,
    target_str
)

from test_framework.messages import (
    CBlock,
    SEQUENCE_FINAL,
    uint256_from_compact,
)

import json
import os

# See data/README.md
COINBASE_SCRIPT_PUBKEY = "76a914eadbac7f36c37e39361168b7aaee3cb24a25312d88ac"
KINGPEPE_MAINNET_RETARGET_INTERVAL = 120
KINGPEPE_MAINNET_INITIAL_N_BITS = 0x1e0ffff0
KINGPEPE_MAINNET_INITIAL_TARGET = uint256_from_compact(KINGPEPE_MAINNET_INITIAL_N_BITS)
KINGPEPE_MAINNET_DIFF_4_N_BITS = 0x1e03fffc
KINGPEPE_MAINNET_DIFF_4_TARGET = uint256_from_compact(KINGPEPE_MAINNET_DIFF_4_N_BITS)


def difficulty_from_bits(nbits):
    """Mirror the getdifficulty RPC calculation for deterministic assertions."""
    shift = (nbits >> 24) & 0xff
    difficulty = 0x0000ffff / (nbits & 0x00ffffff)
    while shift < 29:
        difficulty *= 256.0
        shift += 1
    while shift > 29:
        difficulty /= 256.0
        shift -= 1
    return difficulty


class MiningMainnetTest(BitcoinTestFramework):

    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True
        self.chain = "" # main

    def add_options(self, parser):
        parser.add_argument(
            '--datafile',
            default='data/mainnet_alt.json',
            help='Block data file (default: %(default)s)',
        )

    def mine(self, height, prev_hash, blocks, node):
        self.log.debug(f"height={height}")
        block = CBlock()
        block.nVersion = 0x20000000
        block.hashPrevBlock = int(prev_hash, 16)
        block.nTime = blocks['timestamps'][height - 1]
        if height < KINGPEPE_MAINNET_RETARGET_INTERVAL:
            block.nBits = KINGPEPE_MAINNET_INITIAL_N_BITS
        else:
            block.nBits = KINGPEPE_MAINNET_DIFF_4_N_BITS
        block.nNonce = blocks['nonces'][height - 1]
        block.vtx = [create_coinbase(height=height, script_pubkey=bytes.fromhex(COINBASE_SCRIPT_PUBKEY), halving_period=210000)]
        # The alternate mainnet chain was mined with non-timelocked coinbase txs.
        block.vtx[0].nLockTime = 0
        block.vtx[0].vin[0].nSequence = SEQUENCE_FINAL
        block.hashMerkleRoot = block.calc_merkle_root()
        block_hex = block.serialize(with_witness=False).hex()
        self.log.debug(block_hex)
        assert_equal(node.submitblock(block_hex), None)
        prev_hash = node.getbestblockhash()
        assert_equal(prev_hash, block.hash_hex)
        return prev_hash


    def run_test(self):
        node = self.nodes[0]
        # Clear disk space warning
        node.stderr.seek(0)
        node.stderr.truncate()
        self.log.info("Load alternative mainnet blocks")
        path = os.path.join(os.path.dirname(os.path.realpath(__file__)), self.options.datafile)
        prev_hash = node.getbestblockhash()
        blocks = None
        with open(path) as f:
            blocks = json.load(f)
            n_blocks = len(blocks['timestamps'])
            assert_equal(n_blocks, KINGPEPE_MAINNET_RETARGET_INTERVAL)

        # Mine up to the last block of the first retarget period
        for i in range(KINGPEPE_MAINNET_RETARGET_INTERVAL - 1):
            prev_hash = self.mine(i + 1, prev_hash, blocks, node)

        assert_equal(node.getblockcount(), KINGPEPE_MAINNET_RETARGET_INTERVAL - 1)

        self.log.info("Check difficulty adjustment with getmininginfo")
        mining_info = node.getmininginfo()
        assert_approx(
            mining_info['difficulty'],
            difficulty_from_bits(KINGPEPE_MAINNET_INITIAL_N_BITS),
            vspan=0.000000001,
        )
        assert_equal(mining_info['bits'], nbits_str(KINGPEPE_MAINNET_INITIAL_N_BITS))
        assert_equal(mining_info['target'], target_str(KINGPEPE_MAINNET_INITIAL_TARGET))

        assert_equal(mining_info['next']['height'], KINGPEPE_MAINNET_RETARGET_INTERVAL)
        assert_approx(
            mining_info['next']['difficulty'],
            difficulty_from_bits(KINGPEPE_MAINNET_DIFF_4_N_BITS),
            vspan=0.000000001,
        )
        assert_equal(mining_info['next']['bits'], nbits_str(KINGPEPE_MAINNET_DIFF_4_N_BITS))
        assert_equal(mining_info['next']['target'], target_str(KINGPEPE_MAINNET_DIFF_4_TARGET))

        # Mine first block of the second retarget period
        height = KINGPEPE_MAINNET_RETARGET_INTERVAL
        prev_hash = self.mine(height, prev_hash, blocks, node)
        assert_equal(node.getblockcount(), height)

        mining_info = node.getmininginfo()
        assert_approx(
            mining_info['difficulty'],
            difficulty_from_bits(KINGPEPE_MAINNET_DIFF_4_N_BITS),
            vspan=0.000000001,
        )

        self.log.info("getblock RPC should show historical target")
        block_info = node.getblock(node.getblockhash(1))

        assert_approx(
            block_info['difficulty'],
            difficulty_from_bits(KINGPEPE_MAINNET_INITIAL_N_BITS),
            vspan=0.000000001,
        )
        assert_equal(block_info['bits'], nbits_str(KINGPEPE_MAINNET_INITIAL_N_BITS))
        assert_equal(block_info['target'], target_str(KINGPEPE_MAINNET_INITIAL_TARGET))


if __name__ == '__main__':
    MiningMainnetTest(__file__).main()
