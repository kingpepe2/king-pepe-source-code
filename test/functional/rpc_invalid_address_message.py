#!/usr/bin/env python3
# Copyright (c) 2020-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test error messages for 'getaddressinfo' and 'validateaddress' RPC commands."""

from test_framework.address import byte_to_base58, program_to_witness
from test_framework.segwit_addr import Encoding, bech32_encode, convertbits
from test_framework.test_framework import BitcoinTestFramework

from test_framework.util import (
    assert_equal,
    assert_raises_rpc_error,
)

def _replace_char(string, index, replacement):
    assert string[index] != replacement
    return string[:index] + replacement + string[index + 1:]


BECH32_VALID = program_to_witness(0, bytes.fromhex('5ec3eaf488f054ae43f85c20c96dfb0503715484'))
BECH32_VALID_UNKNOWN_WITNESS = program_to_witness(1, bytes(2))
BECH32_VALID_CAPITALS = program_to_witness(0, bytes.fromhex('0ffeeeb2c2c5fa30844dfcc3d07fc71c2f2d1f91')).upper()
BECH32_VALID_MULTISIG = program_to_witness(0, bytes(32))

BECH32_INVALID_BECH32 = bech32_encode(Encoding.BECH32, 'rkpepe', [1] + convertbits(bytes(32), 8, 5))
BECH32_INVALID_BECH32M = bech32_encode(Encoding.BECH32M, 'rkpepe', [0] + convertbits(bytes(20), 8, 5))
BECH32_INVALID_VERSION = bech32_encode(Encoding.BECH32M, 'rkpepe', [17] + convertbits(bytes(32), 8, 5))
BECH32_INVALID_SIZE = bech32_encode(Encoding.BECH32M, 'rkpepe', [16] + convertbits(bytes(41), 8, 5))
BECH32_INVALID_V0_SIZE = bech32_encode(Encoding.BECH32, 'rkpepe', [0] + convertbits(bytes(21), 8, 5))
BECH32_INVALID_PREFIX = 'bc1pw508d6qejxtdg4y5r3zarvary0c5xw7kw508d6qejxtdg4y5r3zarvary0c5xw7k7grplx'
BECH32_TOO_LONG = 'rkpepe1q049edschfnwystcqnsvyfpj23mpsg3jcedq9xv049edschfnwystcqnsvyfpj23mpsg3jcedq9xv049edschfnwystcqnsvyfpj23m'
BECH32_ONE_ERROR = _replace_char(BECH32_VALID, 9, 'x')
BECH32_ONE_ERROR_CAPITALS = _replace_char(BECH32_VALID_CAPITALS, 38, '2')
BECH32_TWO_ERRORS = _replace_char(_replace_char(BECH32_VALID, 22, 'x'), 43, 'l')
BECH32_NO_SEPARATOR = 'rkpepeq049ldschfnwystcqnsvyfpj23mpsg3jcedq9xv'
BECH32_INVALID_CHAR = 'rkpepe1q04oldschfnwystcqnsvyfpj23mpsg3jcedq9xv'
BECH32_MULTISIG_TWO_ERRORS = _replace_char(_replace_char(BECH32_VALID_MULTISIG, 19, 'x'), 30, 'n')
BECH32_WRONG_VERSION = _replace_char(BECH32_VALID, 7, 'p')

BASE58_VALID = byte_to_base58(bytes(20), 64)
BASE58_INVALID_PREFIX = byte_to_base58(bytes(20), 0)
BASE58_INVALID_CHECKSUM = _replace_char(BASE58_VALID, len(BASE58_VALID) - 1, '1')
BASE58_INVALID_LENGTH = byte_to_base58(bytes(21), 64)

INVALID_ADDRESS = 'asfah14i8fajz0123f'
INVALID_ADDRESS_2 = '1q049ldschfnwystcqnsvyfpj23mpsg3jcedq9xv'

class InvalidAddressErrorMessageTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.num_nodes = 1
        self.uses_wallet = None

    def check_valid(self, addr):
        info = self.nodes[0].validateaddress(addr)
        assert info['isvalid']
        assert 'error' not in info
        assert 'error_locations' not in info

    def check_invalid(self, addr, error_str, error_locations=None):
        res = self.nodes[0].validateaddress(addr)
        assert not res['isvalid']
        assert_equal(res['error'], error_str)
        if error_locations:
            assert_equal(res['error_locations'], error_locations)
        else:
            assert_equal(res['error_locations'], [])

    def test_validateaddress(self):
        # Invalid Bech32
        self.check_invalid(BECH32_INVALID_SIZE, "Invalid Bech32 address program size (41 bytes)")
        self.check_invalid(BECH32_INVALID_PREFIX, 'Invalid or unsupported Segwit (Bech32) or Base58 encoding.')
        self.check_invalid(BECH32_INVALID_BECH32, 'Version 1+ witness address must use Bech32m checksum')
        self.check_invalid(BECH32_INVALID_BECH32M, 'Version 0 witness address must use Bech32 checksum')
        self.check_invalid(BECH32_INVALID_VERSION, 'Invalid Bech32 address witness version')
        self.check_invalid(BECH32_INVALID_V0_SIZE, "Invalid Bech32 v0 address program size (21 bytes), per BIP141")
        self.check_invalid(BECH32_TOO_LONG, 'Bech32 string too long', list(range(90, len(BECH32_TOO_LONG))))
        self.check_invalid(BECH32_ONE_ERROR, 'Invalid Bech32 checksum', [9])
        self.check_invalid(BECH32_TWO_ERRORS, 'Invalid Bech32 checksum', [22, 43])
        self.check_invalid(BECH32_ONE_ERROR_CAPITALS, 'Invalid Bech32 checksum', [38])
        self.check_invalid(BECH32_NO_SEPARATOR, 'Missing separator')
        self.check_invalid(BECH32_INVALID_CHAR, 'Invalid Base 32 character', [10])
        self.check_invalid(BECH32_MULTISIG_TWO_ERRORS, 'Invalid Bech32 checksum', [19, 30])
        self.check_invalid(BECH32_WRONG_VERSION, 'Invalid Bech32 checksum', [7])

        # Valid Bech32
        self.check_valid(BECH32_VALID)
        self.check_valid(BECH32_VALID_UNKNOWN_WITNESS)
        self.check_valid(BECH32_VALID_CAPITALS)
        self.check_valid(BECH32_VALID_MULTISIG)

        # Invalid Base58
        self.check_invalid(BASE58_INVALID_PREFIX, 'Invalid or unsupported Base58-encoded address.')
        self.check_invalid(BASE58_INVALID_CHECKSUM, 'Invalid checksum or length of Base58 address (P2PKH or P2SH)')
        self.check_invalid(BASE58_INVALID_LENGTH, 'Invalid checksum or length of Base58 address (P2PKH or P2SH)')

        # Valid Base58
        self.check_valid(BASE58_VALID)

        # Invalid address format
        self.check_invalid(INVALID_ADDRESS, 'Invalid or unsupported Segwit (Bech32) or Base58 encoding.')
        self.check_invalid(INVALID_ADDRESS_2, 'Invalid or unsupported Segwit (Bech32) or Base58 encoding.')

        node = self.nodes[0]


        if not self.options.usecli:
            # Missing arg returns the help text
            assert_raises_rpc_error(-1, "Return information about the given Kingpepe address.", node.validateaddress)
            # Explicit None is not allowed for required parameters
            assert_raises_rpc_error(-3, "JSON value of type null is not of expected type string", node.validateaddress, None)

    def test_getaddressinfo(self):
        node = self.nodes[0]

        assert_raises_rpc_error(-5, "Invalid Bech32 address program size (41 bytes)", node.getaddressinfo, BECH32_INVALID_SIZE)
        assert_raises_rpc_error(-5, "Invalid or unsupported Segwit (Bech32) or Base58 encoding.", node.getaddressinfo, BECH32_INVALID_PREFIX)
        assert_raises_rpc_error(-5, "Invalid or unsupported Base58-encoded address.", node.getaddressinfo, BASE58_INVALID_PREFIX)
        assert_raises_rpc_error(-5, "Invalid or unsupported Segwit (Bech32) or Base58 encoding.", node.getaddressinfo, INVALID_ADDRESS)
        assert "isscript" not in node.getaddressinfo(BECH32_VALID_UNKNOWN_WITNESS)

    def run_test(self):
        self.test_validateaddress()

        if self.is_wallet_compiled():
            self.init_wallet(node=0)
            self.test_getaddressinfo()


if __name__ == '__main__':
    InvalidAddressErrorMessageTest(__file__).main()
