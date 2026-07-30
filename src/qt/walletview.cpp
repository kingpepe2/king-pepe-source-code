// Copyright (c) 2011-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/walletview.h>

#include <qt/addressbookpage.h>
#include <qt/askpassphrasedialog.h>
#include <qt/clientmodel.h>
#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/addressbookwidget.h>
#include <qt/bitcoinunits.h>
#include <qt/dashboardpage.h>
#include <qt/overviewpage.h>
#include <qt/platformstyle.h>
#include <qt/receivepage.h>
#include <qt/sendpage.h>
#include <qt/receivecoinsdialog.h>
#include <qt/sendcoinsdialog.h>
#include <qt/signverifymessagedialog.h>
#include <qt/transactionspage.h>
#include <qt/transactiontablemodel.h>
#include <qt/transactionview.h>
#include <qt/walletmodel.h>

#include <interfaces/wallet.h>

#include <interfaces/node.h>
#include <node/interface_ui.h>
#include <util/strencodings.h>

#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QString FormatDashboardSyncProgress(double verification_progress)
{
    double percent = verification_progress * 100.0;
    if (percent < 0.0) percent = 0.0;
    if (percent > 100.0) percent = 100.0;
    return QStringLiteral("%1%").arg(QString::number(percent, 'f', percent >= 99.95 ? 0 : 1));
}

QString FormatDashboardSyncStatus(double verification_progress)
{
    return verification_progress >= 0.9995 ? QObject::tr("Synced") : QObject::tr("Syncing");
}
} // namespace

WalletView::WalletView(WalletModel* wallet_model, const PlatformStyle* _platformStyle, QWidget* parent)
    : QStackedWidget(parent),
      walletModel(wallet_model),
      platformStyle(_platformStyle)
{
    assert(walletModel);

    // Create tabs
    overviewPage = new OverviewPage(platformStyle);
    dashboardPage = new DashboardPage(this);
    modernTransactionsPage = new TransactionsPage(this);
    modernReceivePage = new ReceivePage(this);
    sendPage = new SendPage(this);
    overviewPage->setWalletModel(walletModel);

    transactionsPage = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout();
    QHBoxLayout *hbox_buttons = new QHBoxLayout();
    transactionView = new TransactionView(platformStyle, this);
    transactionView->setModel(walletModel);

    vbox->addWidget(transactionView);
    QPushButton *exportButton = new QPushButton(tr("&Export"), this);
    exportButton->setToolTip(tr("Export the data in the current tab to a file"));
    if (platformStyle->getImagesOnButtons()) {
        exportButton->setIcon(platformStyle->SingleColorIcon(":/icons/export"));
    }
    hbox_buttons->addStretch();
    hbox_buttons->addWidget(exportButton);
    vbox->addLayout(hbox_buttons);
    transactionsPage->setLayout(vbox);

    receiveCoinsPage = new ReceiveCoinsDialog(platformStyle);
    receiveCoinsPage->setModel(walletModel);

    sendCoinsPage = new SendCoinsDialog(platformStyle);
    sendCoinsPage->setModel(walletModel);

    usedSendingAddressesPage = new AddressBookPage(platformStyle, AddressBookPage::ForEditing, AddressBookPage::SendingTab, this);
    usedSendingAddressesPage->setModel(walletModel->getAddressTableModel());

    usedReceivingAddressesPage = new AddressBookPage(platformStyle, AddressBookPage::ForEditing, AddressBookPage::ReceivingTab, this);
    usedReceivingAddressesPage->setModel(walletModel->getAddressTableModel());

    addWidget(overviewPage);
    addWidget(dashboardPage);
    addWidget(modernTransactionsPage);
    addWidget(modernReceivePage);
    addWidget(transactionsPage);
    addWidget(receiveCoinsPage);
    // KingPepe: host the proven send dialog inside the modern Send shell.
    sendPage->setSendWidget(sendCoinsPage);
    addWidget(sendPage);

    // KingPepe: route the modern dashboard's actions to the existing pages.
    connect(dashboardPage, &DashboardPage::sendRequested, this, [this] { gotoSendCoinsPage(); });
    connect(dashboardPage, &DashboardPage::receiveRequested, this, &WalletView::gotoReceiveCoinsPage);
    connect(dashboardPage, &DashboardPage::transactionsRequested, this, &WalletView::gotoHistoryPage);
    connect(dashboardPage, &DashboardPage::addressBookRequested, this, &WalletView::usedSendingAddresses);
    dashboardPage->setWalletModel(walletModel);

    // KingPepe: feed live balances into the dashboard, formatted with the display unit.
    auto updateDashboardBalance = [this](const interfaces::WalletBalances& bal) {
        if (!walletModel || !walletModel->getOptionsModel()) return;
        const BitcoinUnit unit = walletModel->getOptionsModel()->getDisplayUnit();
        const CAmount total = bal.balance + bal.unconfirmed_balance + bal.immature_balance;
        dashboardPage->setBalance(
            BitcoinUnits::formatWithUnit(unit, total),
            BitcoinUnits::formatWithUnit(unit, bal.balance),
            BitcoinUnits::formatWithUnit(unit, bal.unconfirmed_balance),
            BitcoinUnits::formatWithUnit(unit, bal.immature_balance));
    };
    connect(walletModel, &WalletModel::balanceChanged, this, updateDashboardBalance);
    updateDashboardBalance(walletModel->getCachedBalance());

    // KingPepe: feed the modern transactions screen from the existing table model.
    modernTransactionsPage->setModel(walletModel);

    // KingPepe: modern address book window (reuses the address table model).
    modernAddressBook = new AddressBookWidget(this);
    modernAddressBook->setModel(walletModel);

    // KingPepe: modern receive screen (reuses address derivation + QR + requests).
    modernReceivePage->setModel(walletModel);

    connect(overviewPage, &OverviewPage::transactionClicked, this, &WalletView::transactionClicked);
    // Clicking on a transaction on the overview pre-selects the transaction on the transaction history page
    connect(overviewPage, &OverviewPage::transactionClicked, transactionView, qOverload<const QModelIndex&>(&TransactionView::focusTransaction));

    connect(overviewPage, &OverviewPage::outOfSyncWarningClicked, this, &WalletView::outOfSyncWarningClicked);

    connect(sendCoinsPage, &SendCoinsDialog::coinsSent, this, &WalletView::coinsSent);
    // Highlight transaction after send
    connect(sendCoinsPage, &SendCoinsDialog::coinsSent, transactionView, qOverload<const Txid&>(&TransactionView::focusTransaction));

    // Clicking on "Export" allows to export the transaction list
    connect(exportButton, &QPushButton::clicked, transactionView, &TransactionView::exportClicked);

    // Pass through messages from sendCoinsPage
    connect(sendCoinsPage, &SendCoinsDialog::message, this, &WalletView::message);
    // Pass through messages from transactionView
    connect(transactionView, &TransactionView::message, this, &WalletView::message);

    connect(this, &WalletView::setPrivacy, overviewPage, &OverviewPage::setPrivacy);
    connect(this, &WalletView::setPrivacy, this, &WalletView::disableTransactionView);

    // Receive and pass through messages from wallet model
    connect(walletModel, &WalletModel::message, this, &WalletView::message);

    // Handle changes in encryption status
    connect(walletModel, &WalletModel::encryptionStatusChanged, this, &WalletView::encryptionStatusChanged);

    // Balloon pop-up for new transaction
    connect(walletModel->getTransactionTableModel(), &TransactionTableModel::rowsInserted, this, &WalletView::processNewTransaction);

    // Ask for passphrase if needed
    connect(walletModel, &WalletModel::requireUnlock, this, &WalletView::unlockWallet);

    // Show progress dialog
    connect(walletModel, &WalletModel::showProgress, this, &WalletView::showProgress);
}

WalletView::~WalletView() = default;

void WalletView::setClientModel(ClientModel *_clientModel)
{
    this->clientModel = _clientModel;

    overviewPage->setClientModel(_clientModel);
    sendCoinsPage->setClientModel(_clientModel);
    walletModel->setClientModel(_clientModel);

    // KingPepe: seed the modern dashboard's live stats from the client model.
    if (_clientModel) {
        dashboardPage->setBlockHeight(QString::number(_clientModel->getNumBlocks()));
        dashboardPage->setConnections(QString::number(_clientModel->getNumConnections()));
        dashboardPage->setNetworkStatus(_clientModel->node().getNetworkActive() ? tr("Online") : tr("Offline"));
        dashboardPage->setSyncProgress(QStringLiteral("100%"), tr("Synced"));
        connect(_clientModel, &ClientModel::numConnectionsChanged, this, [this](int count) {
            dashboardPage->setConnections(QString::number(count));
        });
        connect(_clientModel, &ClientModel::networkActiveChanged, this, [this](bool active) {
            dashboardPage->setNetworkStatus(active ? tr("Online") : tr("Offline"));
        });
        connect(_clientModel, &ClientModel::numBlocksChanged, this,
                [this](int count, const QDateTime&, double verification_progress, SyncType, SynchronizationState) {
                    dashboardPage->setBlockHeight(QString::number(count));
                    dashboardPage->setSyncProgress(FormatDashboardSyncProgress(verification_progress),
                                                   FormatDashboardSyncStatus(verification_progress));
                });
    }
}

void WalletView::processNewTransaction(const QModelIndex& parent, int start, int /*end*/)
{
    // Prevent balloon-spam when initial block download is in progress
    if (!clientModel || clientModel->node().isInitialBlockDownload()) {
        return;
    }

    TransactionTableModel *ttm = walletModel->getTransactionTableModel();
    if (!ttm || ttm->processingQueuedTransactions())
        return;

    QString date = ttm->index(start, TransactionTableModel::Date, parent).data().toString();
    qint64 amount = ttm->index(start, TransactionTableModel::Amount, parent).data(Qt::EditRole).toLongLong();
    QString type = ttm->index(start, TransactionTableModel::Type, parent).data().toString();
    QModelIndex index = ttm->index(start, 0, parent);
    QString address = ttm->data(index, TransactionTableModel::AddressRole).toString();
    QString label = GUIUtil::HtmlEscape(ttm->data(index, TransactionTableModel::LabelRole).toString());

    Q_EMIT incomingTransaction(date, walletModel->getOptionsModel()->getDisplayUnit(), amount, type, address, label, GUIUtil::HtmlEscape(walletModel->getWalletName()));
}

void WalletView::gotoOverviewPage()
{
    // KingPepe: the Overview navigation now presents the modern dashboard.
    setCurrentWidget(dashboardPage);
}

void WalletView::gotoHistoryPage()
{
    // KingPepe: the History navigation now presents the modern transactions screen.
    setCurrentWidget(modernTransactionsPage);
}

void WalletView::gotoReceiveCoinsPage()
{
    // KingPepe: the Receive navigation now presents the modern receive screen.
    setCurrentWidget(modernReceivePage);
}

void WalletView::gotoSendCoinsPage(QString addr)
{
    // KingPepe: present the modern Send shell (which hosts the proven send dialog).
    setCurrentWidget(sendPage);

    if (!addr.isEmpty())
        sendCoinsPage->setAddress(addr);
}

void WalletView::gotoSignMessageTab(QString addr)
{
    // calls show() in showTab_SM()
    SignVerifyMessageDialog *signVerifyMessageDialog = new SignVerifyMessageDialog(platformStyle, this);
    signVerifyMessageDialog->setAttribute(Qt::WA_DeleteOnClose);
    signVerifyMessageDialog->setModel(walletModel);
    signVerifyMessageDialog->showTab_SM(true);

    if (!addr.isEmpty())
        signVerifyMessageDialog->setAddress_SM(addr);
}

void WalletView::gotoVerifyMessageTab(QString addr)
{
    // calls show() in showTab_VM()
    SignVerifyMessageDialog *signVerifyMessageDialog = new SignVerifyMessageDialog(platformStyle, this);
    signVerifyMessageDialog->setAttribute(Qt::WA_DeleteOnClose);
    signVerifyMessageDialog->setModel(walletModel);
    signVerifyMessageDialog->showTab_VM(true);

    if (!addr.isEmpty())
        signVerifyMessageDialog->setAddress_VM(addr);
}

bool WalletView::handlePaymentRequest(const SendCoinsRecipient& recipient)
{
    return sendCoinsPage->handlePaymentRequest(recipient);
}

void WalletView::showOutOfSyncWarning(bool fShow)
{
    overviewPage->showOutOfSyncWarning(fShow);
}

void WalletView::encryptWallet()
{
    auto dlg = new AskPassphraseDialog(AskPassphraseDialog::Encrypt, this);
    dlg->setModel(walletModel);
    connect(dlg, &QDialog::finished, this, &WalletView::encryptionStatusChanged);
    GUIUtil::ShowModalDialogAsynchronously(dlg);
}

void WalletView::backupWallet()
{
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Backup Wallet"), QString(),
        //: Name of the wallet data file format.
        tr("Wallet Data") + QLatin1String(" (*.dat)"), nullptr);

    if (filename.isEmpty())
        return;

    if (!walletModel->wallet().backupWallet(filename.toLocal8Bit().data())) {
        Q_EMIT message(tr("Backup Failed"), tr("There was an error trying to save the wallet data to %1.").arg(filename),
            CClientUIInterface::MSG_ERROR);
        }
    else {
        Q_EMIT message(tr("Backup Successful"), tr("The wallet data was successfully saved to %1.").arg(filename),
            CClientUIInterface::MSG_INFORMATION);
    }
}

void WalletView::changePassphrase()
{
    auto dlg = new AskPassphraseDialog(AskPassphraseDialog::ChangePass, this);
    dlg->setModel(walletModel);
    GUIUtil::ShowModalDialogAsynchronously(dlg);
}

void WalletView::unlockWallet()
{
    // Unlock wallet when requested by wallet model
    if (walletModel->getEncryptionStatus() == WalletModel::Locked) {
        AskPassphraseDialog dlg(AskPassphraseDialog::Unlock, this);
        dlg.setModel(walletModel);
        // A modal dialog must be synchronous here as expected
        // in the WalletModel::requestUnlock() function.
        dlg.exec();
    }
}

void WalletView::usedSendingAddresses()
{
    // KingPepe: present the modern custom address book.
    GUIUtil::bringToFront(modernAddressBook);
}

void WalletView::usedReceivingAddresses()
{
    // KingPepe: present the modern custom address book.
    GUIUtil::bringToFront(modernAddressBook);
}

void WalletView::showProgress(const QString &title, int nProgress)
{
    if (nProgress == 0) {
        progressDialog = new QProgressDialog(title, tr("Cancel"), 0, 100);
        GUIUtil::PolishProgressDialog(progressDialog);
        progressDialog->setWindowModality(Qt::ApplicationModal);
        progressDialog->setAutoClose(false);
        progressDialog->setValue(0);
    } else if (nProgress == 100) {
        if (progressDialog) {
            progressDialog->close();
            progressDialog->deleteLater();
            progressDialog = nullptr;
        }
    } else if (progressDialog) {
        if (progressDialog->wasCanceled()) {
            getWalletModel()->wallet().abortRescan();
        } else {
            progressDialog->setValue(nProgress);
        }
    }
}

void WalletView::disableTransactionView(bool disable)
{
    transactionView->setDisabled(disable);
}
