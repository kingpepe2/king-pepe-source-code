# Copyright (c) 2023-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or https://opensource.org/license/mit/.

function(generate_setup_nsi)
  set(abs_top_srcdir ${PROJECT_SOURCE_DIR})
  set(abs_top_builddir ${PROJECT_BINARY_DIR})
  set(CLIENT_URL ${PROJECT_HOMEPAGE_URL})
  set(CLIENT_TARNAME "kingpepe")
  set(BITCOIN_WRAPPER_NAME "kingpepe")
  set(BITCOIN_GUI_NAME "kingpepe-qt")
  set(BITCOIN_DAEMON_NAME "kingpeped")
  set(BITCOIN_CLI_NAME "kingpepe-cli")
  set(BITCOIN_TX_NAME "kingpepe-tx")
  set(BITCOIN_WALLET_TOOL_NAME "kingpepe-wallet")
  set(BITCOIN_TEST_NAME "test_kingpepe")
  set(EXEEXT ${CMAKE_EXECUTABLE_SUFFIX})
  configure_file(${PROJECT_SOURCE_DIR}/share/setup.nsi.in ${PROJECT_BINARY_DIR}/kingpepe-win64-setup.nsi USE_SOURCE_PERMISSIONS @ONLY)
endfunction()
