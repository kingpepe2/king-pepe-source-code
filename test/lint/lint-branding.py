#!/usr/bin/env python3
#
# Copyright (c) 2024-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

"""
Guard against reintroducing user-visible Bitcoin product branding.

KingPepe Core is a rebrand of Bitcoin Core. A large amount of *internal*
Bitcoin naming is legitimately retained (copyright/attribution, upstream
URLs, BIP references, the PSBT acronym, internal build targets/libraries
such as test_bitcoin/bench_bitcoin/bitcoin_* and the public libbitcoinkernel
API, and the test-framework environment-variable keys BITCOIND/BITCOINCLI...).

This linter therefore does NOT flag every occurrence of "bitcoin". It only
flags a curated set of *user-visible / CI-facing* branding leaks in the paths
where product branding is shown to users or in CI output, so that a debrand
regression fails fast with an exact file:line. The allowlist keeps legitimate
upstream/internal references from producing false positives.
"""

import re
import subprocess
import sys

# Paths whose contents are user-visible product/CI branding. Only files under
# these prefixes are scanned. Kept narrow on purpose to avoid false positives
# from internal source (namespaces, class names, include guards, etc.).
SCOPED_PREFIXES = (
    ".github/workflows/",
    "ci/",
    "src/qt/",
    "src/test/main.cpp",
    "src/test/kernel/test_kernel.cpp",
    "src/qt/test/CMakeLists.txt",
    "src/bench/",
    "src/bitcoin-cli.cpp",
)

# Prohibited user-visible branding. Each is (compiled_regex, human_reason).
# These are the specific product-branding strings that must stay KingPepe.
PROHIBITED = [
    (re.compile(r"Bitcoin Core Test Suite"), "test suite label must be 'KingPepe Core Test Suite'"),
    (re.compile(r"Bitcoin Kernel Test Suite"), "test suite label must be 'KingPepe Kernel Test Suite'"),
    (re.compile(r"\bbitcoin-linter\b"), "CI container name must be 'kingpepe-linter'"),
    (re.compile(r"add_test\(NAME\s+test_bitcoin\b"), "ctest label must be test_kingpepe* (binary target may stay test_bitcoin)"),
    (re.compile(r'tr\("Bitcoin"\)'), "user-visible GUI string must say KingPepe"),
    (re.compile(r"/Satoshi:\d"), "example/agent subversion must use KingPepe (/Kingpepe:)"),
    # Default datadir guard/paths shown to users must target the KingPepe datadir.
    (re.compile(r'(HOME.{0,4}/\.bitcoin\b)|(Application Support/Bitcoin\b)'), "default datadir must be KingPepe (.kingpepe / Application Support/Kingpepe)"),
]

# Lines matching any of these are legitimate and never flagged, even if they
# also match a prohibited pattern. Keep tightly scoped and documented.
ALLOWLIST = [
    re.compile(r"The Bitcoin Core developers"),          # copyright/attribution
    re.compile(r"opensource\.org|mit-license"),          # license text
    re.compile(r"github\.com/bitcoin"),                  # upstream repo URLs
    re.compile(r"bitcoincore\.org"),                     # upstream homepage
    re.compile(r"bitcoin\.stackexchange\.com"),          # upstream support link
    re.compile(r"BIP\d"),                                # BIP references
    re.compile(r"Partially Signed Bitcoin Transaction"), # PSBT protocol-standard term
    re.compile(r"BITCOIN[A-Z_]*\s*[:=]"),                # test-framework env-var keys / config keys
    re.compile(r"'bitcoin/bitcoin'|bitcoin-core/"),      # upstream repo slugs (warp runners, qa-assets)
]


def get_scoped_files():
    out = subprocess.run(
        ["git", "ls-files", "--", *SCOPED_PREFIXES],
        stdout=subprocess.PIPE, text=True, check=True,
    ).stdout
    return [f for f in out.splitlines() if f]


def main():
    violations = []
    for path in get_scoped_files():
        try:
            with open(path, "r", encoding="utf-8") as fh:
                lines = fh.readlines()
        except (UnicodeDecodeError, FileNotFoundError):
            continue
        for lineno, line in enumerate(lines, start=1):
            if any(alw.search(line) for alw in ALLOWLIST):
                continue
            for pat, reason in PROHIBITED:
                if pat.search(line):
                    violations.append((path, lineno, reason, line.rstrip()))

    if violations:
        print("Prohibited user-visible Bitcoin branding found "
              "(rebrand to KingPepe or extend the allowlist if legitimate):\n")
        for path, lineno, reason, text in violations:
            print(f"{path}:{lineno}: {reason}")
            print(f"    {text}")
        sys.exit(1)

    sys.exit(0)


if __name__ == "__main__":
    main()
