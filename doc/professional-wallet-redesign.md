# KingPepe Desktop Wallet — Professional Redesign (Audit & Plan)

Branch: `professional-desktop-wallet` (off `main` @ 4a99ac5)
Scope: **UI/UX, architecture cleanup, reliability, branding only.** No changes to consensus,
transactions, address generation, `wallet.dat` format, RPC, mining, premine, chain params, or balances.

## 1. What the wallet is (audit result)

The KingPepe desktop wallet is a **standard Bitcoin Core Qt wallet** (`src/qt/`, 59 `.cpp`, 19 `.ui`
forms, Qt Widgets). There is **no dedicated theme/QSS** today — it renders in default Qt styling with a
few inline `setStyleSheet` calls. A working `kingpepe-qt.exe` already ships, so the backend/build is proven.

### Screen → file map (backend reused as-is; only view layers change)
| Screen / area | Form (`src/qt/forms/`) | Logic (`src/qt/`) | Model (reused unchanged) |
|---|---|---|---|
| Dashboard | `overviewpage.ui` | `overviewpage.cpp` | `WalletModel`, `ClientModel` |
| Send | `sendcoinsdialog.ui`, `sendcoinsentry.ui` | `sendcoinsdialog.cpp`, `coincontroldialog.cpp` | `WalletModel`, `CoinControl` |
| Receive | `receivecoinsdialog.ui`, `receiverequestdialog.ui` | `receivecoinsdialog.cpp`, `qrimagewidget.cpp` | `RecentRequestsTableModel` |
| Transactions | (embedded) | `transactionview.cpp`, `transactiontablemodel.cpp` | `TransactionTableModel` |
| Address book | `addressbookpage.ui`, `editaddressdialog.ui` | `addressbookpage.cpp` | `AddressTableModel` |
| Settings | `optionsdialog.ui` | `optionsdialog.cpp` | `OptionsModel` |
| Network/peers | `debugwindow.ui` | `rpcconsole.cpp` | `PeerTableModel`, `BanTableModel` |
| Wallet security | `askpassphrasedialog.ui`, `createwalletdialog.ui` | `askpassphrasedialog.cpp` | `WalletModel` |
| Shell/nav | — | `bitcoingui.cpp`, `walletframe.cpp`, `walletview.cpp` | — |
| Styling infra | — | `platformstyle.cpp`, `guiutil.cpp`, `res/bitcoin.qrc` | — |

## 2. Files to be modified vs added

**Modified (view only):** `bitcoingui.cpp` (nav rail, theme load), `walletframe/walletview` (page host),
the 8 core `.ui` forms above, `overviewpage.cpp`, `optionsdialog.cpp/.ui` (add Display/Explorer/theme),
`guiutil.cpp`/`platformstyle.cpp` (theme + HiDPI helpers), `res/bitcoin.qrc` (register theme/fonts).

**Added (new):** `src/qt/res/themes/dark.qss` + `light.qss`, `src/qt/thememanager.{h,cpp}`,
`src/qt/res/branding/` (logo/splash/about art from the official `$kingpepe` logo), a `NavigationBar`
widget, dashboard "card" widgets, `res/fonts/` (an open-licensed font, e.g. Inter, embedded via qrc).

## 3. Security risks & mitigations
- Theme QSS is static/resource-embedded — no remote CSS, no `eval`, no network. ✅
- Do **not** add private-key/seed export to the UI beyond what already exists (dumpprivkey stays in console only). ✅
- Inactivity auto-lock: implement via a Qt idle timer that calls the **existing** `WalletModel::setWalletLocked` — no new crypto. ✅
- Explorer links open the OS browser with a URL built from txid only — never keys; feature is optional/off by default. ✅
- No telemetry/analytics/external calls added. Sensitive fields stay masked; destructive actions keep confirmations.

## 4. Compatibility risks & mitigations
- **Zero backend edits** — `wallet.dat`, descriptors, RPC, chainparams, addresses untouched → existing wallets/nodes fully compatible. ✅
- `.ui` edits are layout/style only; signal/slot wiring to models preserved.
- New settings keys are additive (QSettings) with safe defaults; existing config/data dir untouched.
- Old wallet is not deleted; redesign lives on a branch until it builds + passes compatibility tests.

## 5. Phased plan (matches request)
- **P1** Theme system (QSS dark+light, ThemeManager), navigation rail, dashboard cards, branding.
- **P2** Send, Receive, Transactions, Address book.
- **P3** Settings (categorized), Network/sync panel, wallet security, backup UI.
- **P4** HiDPI, error handling, accessibility, performance, Windows packaging + version metadata.

## 6. ⚠️ Build/test/screenshot feasibility (critical)
Completion bar = "compiled, launched, tested, screenshots." This environment has **CMake + MSVC 2022 but
NO Qt SDK** and none of the Bitcoin Core deps (Boost, libevent, BerkeleyDB, qrencode) installed. Building a
Bitcoin Core Qt wallet on Windows requires a full Qt + vcpkg dependency toolchain and a multi-hour compile.
**Therefore compiled screenshots cannot be produced in this session.** Delivery model options are in the plan message.
