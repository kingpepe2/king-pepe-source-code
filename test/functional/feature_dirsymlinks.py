#!/usr/bin/env python3
# Copyright (c) 2022-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.
"""Test successful startup with symlinked directories.
"""

import os

from test_framework.test_framework import BitcoinTestFramework, SkipTest


def assert_directory_symlink_support(base_path):
    target = base_path / "symlink_probe_target"
    link = base_path / "symlink_probe_link"
    os.mkdir(target)
    try:
        os.symlink(target, link, target_is_directory=True)
    except OSError as e:
        if os.name == "nt" and getattr(e, "winerror", None) == 1314:
            raise SkipTest("Windows symlink privilege is not available in this session")
        raise
    finally:
        if os.path.islink(link):
            os.unlink(link)
        if os.path.isdir(target):
            os.rmdir(target)


def rename_and_link(*, from_name, to_name):
    os.rename(from_name, to_name)
    os.symlink(to_name, from_name, target_is_directory=True)
    assert os.path.islink(from_name) and os.path.isdir(from_name)


class SymlinkTest(BitcoinTestFramework):
    def set_test_params(self):
        self.num_nodes = 1

    def run_test(self):
        dir_new_blocks = self.nodes[0].chain_path / "new_blocks"
        dir_new_chainstate = self.nodes[0].chain_path / "new_chainstate"
        assert_directory_symlink_support(self.nodes[0].chain_path)
        self.stop_node(0)

        rename_and_link(
            from_name=self.nodes[0].blocks_path,
            to_name=dir_new_blocks,
        )
        rename_and_link(
            from_name=self.nodes[0].chain_path / "chainstate",
            to_name=dir_new_chainstate,
        )

        self.start_node(0)


if __name__ == "__main__":
    SymlinkTest(__file__).main()
