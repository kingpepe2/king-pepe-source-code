// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/transactionspage.h>

#include <qt/transactiontablemodel.h>
#include <qt/walletmodel.h>

#include <QAbstractItemModel>
#include <QDateTime>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QScrollArea>
#include <QVBoxLayout>

TransactionsPage::TransactionsPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("transactionsPage"));

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    QLabel* heading = new QLabel(tr("Transactions"), this);
    heading->setObjectName(QStringLiteral("pageHeading"));
    root->addWidget(heading);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget* container = new QWidget(scroll);
    m_list = new QVBoxLayout(container);
    m_list->setContentsMargins(0, 0, 0, 0);
    m_list->setSpacing(8);
    m_list->addStretch(1); // rows are inserted above this stretch
    scroll->setWidget(container);

    root->addWidget(scroll);
}

void TransactionsPage::setModel(WalletModel* model)
{
    m_walletModel = model;
    if (!model) return;
    TransactionTableModel* ttm = model->getTransactionTableModel();
    if (!ttm) return;

    connect(ttm, &QAbstractItemModel::modelReset, this, &TransactionsPage::rebuild);
    connect(ttm, &QAbstractItemModel::rowsInserted, this, &TransactionsPage::rebuild);
    connect(ttm, &QAbstractItemModel::rowsRemoved, this, &TransactionsPage::rebuild);
    connect(ttm, &QAbstractItemModel::dataChanged, this, &TransactionsPage::rebuild);
    connect(ttm, &QAbstractItemModel::layoutChanged, this, &TransactionsPage::rebuild);

    rebuild();
}

void TransactionsPage::rebuild()
{
    if (!m_walletModel || !m_list) return;
    TransactionTableModel* ttm = m_walletModel->getTransactionTableModel();
    if (!ttm) return;

    // Remove existing row widgets, keeping the trailing stretch (last item).
    while (m_list->count() > 1) {
        QLayoutItem* item = m_list->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    const int rows = ttm->rowCount(QModelIndex());
    const int start = rows > MAX_ROWS ? rows - MAX_ROWS : 0;
    for (int r = rows - 1; r >= start; --r) { // newest first
        const QModelIndex idx = ttm->index(r, 0);
        const QString label = idx.data(TransactionTableModel::LabelRole).toString();
        const QString address = idx.data(TransactionTableModel::AddressRole).toString();
        const QString amount = idx.data(TransactionTableModel::FormattedAmountRole).toString();
        const QString status = idx.data(TransactionTableModel::StatusRole).toString();
        const QDateTime date = idx.data(TransactionTableModel::DateRole).toDateTime();

        QFrame* row = new QFrame();
        row->setObjectName(QStringLiteral("txRow"));
        QHBoxLayout* h = new QHBoxLayout(row);
        h->setContentsMargins(14, 10, 14, 10);
        h->setSpacing(12);

        QLabel* who = new QLabel(label.isEmpty() ? address : label, row);
        who->setObjectName(QStringLiteral("txWho"));
        QLabel* when = new QLabel(date.isValid() ? date.toString(QStringLiteral("yyyy-MM-dd hh:mm")) : status, row);
        when->setObjectName(QStringLiteral("txWhen"));
        QLabel* amt = new QLabel(amount, row);
        amt->setObjectName(amount.startsWith('-') ? QStringLiteral("txAmountOut") : QStringLiteral("txAmountIn"));

        h->addWidget(who);
        h->addWidget(when);
        h->addStretch(1);
        h->addWidget(amt);

        m_list->insertWidget(m_list->count() - 1, row);
    }
}
