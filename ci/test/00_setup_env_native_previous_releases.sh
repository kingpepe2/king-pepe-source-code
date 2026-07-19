#!/usr/bin/env bash
#
# Copyright (c) 2019-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

export LC_ALL=C.UTF-8

export CONTAINER_NAME=ci_native_previous_releases
export CI_IMAGE_NAME_TAG="mirror.gcr.io/ubuntu:22.04"
# Use minimum supported python3.10 and gcc-12, see doc/dependencies.md
export PACKAGES="gcc-12 g++-12 python3-zmq"
export DEP_OPTS="CC=gcc-12 CXX=g++-12"
# Run the extended functional suite (excluding the very slow dbcrash test).
#
# The cross-version backward-compatibility tests require binaries of PRIOR
# KingPepe releases. No such binaries exist yet: the repository has a single
# release (v1.0.0 / "KingPepe Core 31.1") with no published binary assets, so
# there is nothing compatible to test against. Bitcoin Core release binaries are
# NOT compatible with KingPepe (different genesis, network magic, and address
# rules) and must not be used, so previous releases are neither downloaded nor
# enabled here. The affected tests (wallet_backwards_compatibility,
# wallet_migration, mempool_compatibility, feature_unsupported_utxo_db,
# feature_coinstatsindex_compatibility) then skip via skip_if_no_previous_releases;
# feature_presegwit_node_upgrade still runs (it uses the current binary with
# pre-segwit block data, not a prior-release binary).
#
# To restore real backward-compatibility coverage, publish prior KingPepe release
# binaries + sha256 hashes, add them to test/get_previous_releases.py, set
# DOWNLOAD_PREVIOUS_RELEASES=true, and re-add --previous-releases below.
export TEST_RUNNER_EXTRA="--coverage --extended --exclude feature_dbcrash"
export GOAL="install"
export CI_LIMIT_STACK_SIZE=1
export DOWNLOAD_PREVIOUS_RELEASES="false"
# Use -Werror as the CMake version does not support CMAKE_COMPILE_WARNING_AS_ERROR
export BITCOIN_CONFIG="\
 --preset=dev-mode \
 -DREDUCE_EXPORTS=ON \
 -DCMAKE_BUILD_TYPE=Debug \
 -DCMAKE_C_FLAGS='-funsigned-char -Werror' \
 -DCMAKE_C_FLAGS_DEBUG='-g2 -O2' \
 -DCMAKE_CXX_FLAGS='-funsigned-char -Werror' \
 -DCMAKE_CXX_FLAGS_DEBUG='-g2 -O2' \
 -DAPPEND_CPPFLAGS='-DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE' \
"
