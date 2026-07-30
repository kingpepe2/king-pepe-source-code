// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_TRANSACTIONSPAGE_H
#define BITCOIN_QT_TRANSACTIONSPAGE_H

#include <QWidget>

class WalletModel;

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

/**
 * KingPepe modern transactions screen.
 *
 * A fully custom presentation that reuses the existing TransactionTableModel as
 * its data source (backend untouched): it renders the most recent transactions
 * as modern rows (who / date / amount / status). No consensus, wallet, or RPC
 * logic is duplicated; it only reads formatted values from the model.
 */
class TransactionsPage : public QWidget
{
    Q_OBJECT

public:
    explicit TransactionsPage(QWidget* parent = nullptr);

    void setModel(WalletModel* model);

private Q_SLOTS:
    void rebuild();

private:
    WalletModel* m_walletModel{nullptr};
    QVBoxLayout* m_list{nullptr};

    static constexpr int MAX_ROWS = 200;
};

#endif // BITCOIN_QT_TRANSACTIONSPAGE_H
