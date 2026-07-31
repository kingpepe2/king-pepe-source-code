#!/usr/bin/env python3
# Copyright (c) 2014-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test coinbase transactions return the correct categories.

Tests listtransactions, listsinceblock, and gettransaction.
"""

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import (
    assert_array_result
)

class CoinbaseCategoryTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1
        self.setup_clean_chain = True

    def skip_test_if_missing_module(self):
        self.skip_if_no_wallet()

    def assert_category(self, category, address, txid, skip):
        assert_array_result(self.nodes[0].listtransactions(skip=skip),
                            {"address": address},
                            {"category": category})
        assert_array_result(self.nodes[0].listsinceblock()["transactions"],
                            {"address": address},
                            {"category": category})
        assert_array_result(self.nodes[0].gettransaction(txid)["details"],
                            {"address": address},
                            {"category": category})

    def run_test(self):
        # Generate one block to an address
        address = self.nodes[0].getnewaddress()
        self.generatetoaddress(self.nodes[0], 1, address)
        hash = self.nodes[0].getbestblockhash()
        txid = self.nodes[0].getblock(hash)["tx"][0]

        # Coinbase transaction is immature after 1 confirmation
        self.assert_category("immature", address, txid, 0)

        # Mine another 19 blocks on top (KingPepe coinbase maturity is 20; a coinbase
        # matures once its depth exceeds COINBASE_MATURITY, i.e. at 21 confirmations)
        self.generate(self.nodes[0], 19)
        # Coinbase transaction is still immature after 20 confirmations
        self.assert_category("immature", address, txid, 19)

        # Mine one more block
        self.generate(self.nodes[0], 1)
        # Coinbase transaction is now matured after 21 confirmations, so category is "generate"
        self.assert_category("generate", address, txid, 20)

        # Orphan block that paid to address
        self.nodes[0].invalidateblock(hash)
        # Coinbase transaction is now orphaned
        self.assert_category("orphan", address, txid, 20)

if __name__ == '__main__':
    CoinbaseCategoryTest(__file__).main()
