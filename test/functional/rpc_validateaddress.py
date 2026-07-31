#!/usr/bin/env python3
# Copyright (c) 2023-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test validateaddress for main chain"""

from test_framework.script_util import program_to_witness_script
from test_framework.segwit_addr import Encoding, bech32_encode, convertbits
from test_framework.test_framework import BitcoinTestFramework

from test_framework.util import assert_equal

MAIN_HRP = "kpepe"


def witness_address(version, program, encoding=None):
    if encoding is None:
        encoding = Encoding.BECH32 if version == 0 else Encoding.BECH32M
    return bech32_encode(encoding, MAIN_HRP, [version] + convertbits(program, 8, 5))


def witness_script_pubkey(version, program):
    return program_to_witness_script(version, program).hex()


def replace_char(string, index, replacement):
    assert string[index] != replacement
    return string[:index] + replacement + string[index + 1:]


VALID_V0_P2WPKH_PROGRAM = bytes.fromhex("751e76e8199196d454941c45d1b3a323f1433bd6")
VALID_V0_P2WSH_PROGRAM = bytes.fromhex("1863143c14c5166804bd19203356da136c985678cd4d27a1b8c6329604903262")
VALID_V1_LONG_PROGRAM = VALID_V0_P2WPKH_PROGRAM + VALID_V0_P2WPKH_PROGRAM
VALID_V16_SHORT_PROGRAM = bytes.fromhex("751e")
VALID_V2_PROGRAM = bytes.fromhex("751e76e8199196d454941c45d1b3a323")
VALID_V0_ZERO_PROGRAM = bytes.fromhex("000000c4a5cad46221b2a187905e5266362b99d5e91c6ce24d165dab93e86433")
VALID_V1_ZERO_PROGRAM = VALID_V0_ZERO_PROGRAM
VALID_V1_PUBKEY_PROGRAM = bytes.fromhex("79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798")
VALID_P2A_PROGRAM = bytes.fromhex("4e73")

VALID_V0 = witness_address(0, VALID_V0_P2WPKH_PROGRAM)
INVALID_CHECKSUM_INDEX = len(VALID_V0) - 3
INVALID_CHECKSUM = replace_char(VALID_V0, INVALID_CHECKSUM_INDEX, "x")
MIXED_CASE = "k" + VALID_V0.upper()[1:]
MIXED_CASE_LOCATIONS = [1, 2, 3, 4, 6, 7, 11, 13, 14, 15, 16, 17, 18, 19, 21, 23, 25, 26, 27, 28, 29, 30, 31, 33, 35, 36, 38, 39, 40, 41, 42, 44]
INVALID_BASE32_INDEX = len(witness_address(1, VALID_V1_PUBKEY_PROGRAM)) - 2
INVALID_BASE32 = replace_char(witness_address(1, VALID_V1_PUBKEY_PROGRAM), INVALID_BASE32_INDEX, "b")

INVALID_DATA = [
    (
        "tc1qw508d6qejxtdg4y5r3zarvary0c5xw7kg3g4ty",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # Invalid HRP
        [],
    ),
    (INVALID_CHECKSUM, "Invalid Bech32 checksum", [INVALID_CHECKSUM_INDEX]),
    (
        witness_address(1, VALID_V0_P2WPKH_PROGRAM, Encoding.BECH32).upper(),
        "Version 1+ witness address must use Bech32m checksum",
        [],
    ),
    (
        witness_address(3, bytes(1), Encoding.BECH32),
        "Version 1+ witness address must use Bech32m checksum",  # Invalid program length
        [],
    ),
    (
        witness_address(15, VALID_V1_LONG_PROGRAM, Encoding.BECH32),
        "Version 1+ witness address must use Bech32m checksum",  # Invalid program length
        [],
    ),
    (
        witness_address(0, bytes(16)).upper(),
        "Invalid Bech32 v0 address program size (16 bytes), per BIP141",
        [],
    ),
    (
        "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3q0sL5k7",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # tb1, Mixed case
        [],
    ),
    (
        MIXED_CASE,
        "Invalid character or mixed case",  # bc1, Mixed case, not in BIP 173 test vectors
        MIXED_CASE_LOCATIONS,
    ),
    (
        witness_address(2, bytes(16), Encoding.BECH32),
        "Version 1+ witness address must use Bech32m checksum",  # Wrong padding
        [],
    ),
    (
        "tb1qrp33g0q5c5txsp9arysrx4k6zdkfs4nce4xj0gdcccefvpysxf3pjxtptv",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # tb1, Non-zero padding in 8-to-5 conversion
        [],
    ),
    (bech32_encode(Encoding.BECH32, MAIN_HRP, []), "Empty Bech32 data section", []),
    (
        "tc1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq5zuyut",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # Invalid human-readable part
        [],
    ),
    (
        witness_address(1, VALID_V1_PUBKEY_PROGRAM, Encoding.BECH32),
        "Version 1+ witness address must use Bech32m checksum",  # Invalid checksum (Bech32 instead of Bech32m)
        [],
    ),
    (
        "tb1z0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vqglt7rf",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # tb1, Invalid checksum (Bech32 instead of Bech32m)
        [],
    ),
    (
        witness_address(16, VALID_V1_PUBKEY_PROGRAM, Encoding.BECH32).upper(),
        "Version 1+ witness address must use Bech32m checksum",  # Invalid checksum (Bech32 instead of Bech32m)
        [],
    ),
    (
        witness_address(0, VALID_V0_P2WPKH_PROGRAM, Encoding.BECH32M),
        "Version 0 witness address must use Bech32 checksum",  # Invalid checksum (Bech32m instead of Bech32)
        [],
    ),
    (
        "tb1q0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq24jc47",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # tb1, Invalid checksum (Bech32m instead of Bech32)
        [],
    ),
    (
        INVALID_BASE32,
        "Invalid Base 32 character",  # Invalid character in checksum
        [INVALID_BASE32_INDEX],
    ),
    (
        bech32_encode(Encoding.BECH32M, MAIN_HRP, [17] + convertbits(VALID_V1_PUBKEY_PROGRAM, 8, 5)).upper(),
        "Invalid Bech32 address witness version",
        [],
    ),
    (bech32_encode(Encoding.BECH32M, MAIN_HRP, [1] + convertbits(bytes(1), 8, 5)), "Invalid Bech32 address program size (1 byte)", []),
    (
        bech32_encode(Encoding.BECH32M, MAIN_HRP, [1] + convertbits(bytes(41), 8, 5)),
        "Invalid Bech32 address program size (41 bytes)",
        [],
    ),
    (
        witness_address(0, bytes(16)).upper(),
        "Invalid Bech32 v0 address program size (16 bytes), per BIP141",
        [],
    ),
    (
        "tb1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vq47Zagq",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # tb1, Mixed case
        [],
    ),
    (
        bech32_encode(Encoding.BECH32M, MAIN_HRP, [1] + convertbits(VALID_V1_PUBKEY_PROGRAM, 8, 5) + [0, 0]),
        "Invalid padding in Bech32 data section",  # zero padding of more than 4 bits
        [],
    ),
    (
        "tb1p0xlxvlhemja6c4dqv22uapctqupfhlxm9h8z3k2e72q4k9hcz7vpggkg4j",
        "Invalid or unsupported Segwit (Bech32) or Base58 encoding.",  # tb1, Non-zero padding in 8-to-5 conversion
        [],
    ),
    (bech32_encode(Encoding.BECH32, MAIN_HRP, []), "Empty Bech32 data section", []),
]
VALID_DATA = [
    (
        witness_address(0, VALID_V0_P2WPKH_PROGRAM).upper(),
        witness_script_pubkey(0, VALID_V0_P2WPKH_PROGRAM),
    ),
    (
        witness_address(0, VALID_V0_P2WSH_PROGRAM),
        witness_script_pubkey(0, VALID_V0_P2WSH_PROGRAM),
    ),
    (
        witness_address(1, VALID_V1_LONG_PROGRAM),
        witness_script_pubkey(1, VALID_V1_LONG_PROGRAM),
    ),
    (witness_address(16, VALID_V16_SHORT_PROGRAM).upper(), witness_script_pubkey(16, VALID_V16_SHORT_PROGRAM)),
    (witness_address(2, VALID_V2_PROGRAM), witness_script_pubkey(2, VALID_V2_PROGRAM)),
    (
        witness_address(0, VALID_V0_ZERO_PROGRAM),
        witness_script_pubkey(0, VALID_V0_ZERO_PROGRAM),
    ),
    (
        witness_address(1, VALID_V1_ZERO_PROGRAM),
        witness_script_pubkey(1, VALID_V1_ZERO_PROGRAM),
    ),
    (
        witness_address(1, VALID_V1_PUBKEY_PROGRAM),
        witness_script_pubkey(1, VALID_V1_PUBKEY_PROGRAM),
    ),
    (
        witness_address(1, VALID_P2A_PROGRAM),
        witness_script_pubkey(1, VALID_P2A_PROGRAM),
    ),
]


class ValidateAddressMainTest(BitcoinTestFramework):
    def set_test_params(self):
        self.setup_clean_chain = True
        self.chain = ""  # main
        self.num_nodes = 1
        self.extra_args = [["-prune=899"]] * self.num_nodes

    def check_valid(self, addr, spk):
        info = self.nodes[0].validateaddress(addr)
        assert_equal(info["isvalid"], True)
        assert_equal(info["scriptPubKey"], spk)
        assert "error" not in info
        assert "error_locations" not in info

    def check_invalid(self, addr, error_str, error_locations):
        res = self.nodes[0].validateaddress(addr)
        assert_equal(res["isvalid"], False)
        assert_equal(res["error"], error_str)
        assert_equal(res["error_locations"], error_locations)

    def test_validateaddress(self):
        for (addr, error, locs) in INVALID_DATA:
            self.check_invalid(addr, error, locs)
        for (addr, spk) in VALID_DATA:
            self.check_valid(addr, spk)

    def run_test(self):
        self.test_validateaddress()


if __name__ == "__main__":
    ValidateAddressMainTest(__file__).main()
