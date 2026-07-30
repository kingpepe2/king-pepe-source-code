// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_DASHBOARDPAGE_H
#define BITCOIN_QT_DASHBOARDPAGE_H

#include <QWidget>

class WalletModel;

QT_BEGIN_NAMESPACE
class QLabel;
class QVBoxLayout;
QT_END_NAMESPACE

/**
 * KingPepe compact Home screen (Phantom/Backpack-style vertical layout).
 *
 * A scrollable stack of rounded cards: balance, quick actions, assets, recent
 * activity, network status, and sync status. It holds no wallet state: the host feeds it
 * formatted strings from the existing WalletModel/ClientModel and passes the
 * WalletModel only so the recent-activity list can read the existing
 * TransactionTableModel. Emits action signals for the host to route.
 */
class DashboardPage : public QWidget
{
    Q_OBJECT

public:
    explicit DashboardPage(QWidget* parent = nullptr);

    void setBalance(const QString& total, const QString& available,
                    const QString& pending, const QString& immature);
    void setBlockHeight(const QString& height);
    void setConnections(const QString& peers);
    void setNetworkStatus(const QString& status);
    void setSyncProgress(const QString& progress, const QString& status);

    //! Provide the wallet model so the recent-activity list can read transactions.
    void setWalletModel(WalletModel* model);

Q_SIGNALS:
    void sendRequested();
    void receiveRequested();
    void transactionsRequested();
    void addressBookRequested();

private Q_SLOTS:
    void rebuildActivity();

private:
    QLabel* m_total{nullptr};
    QLabel* m_available{nullptr};
    QLabel* m_pending{nullptr};
    QLabel* m_immature{nullptr};
    QLabel* m_sync{nullptr};
    QLabel* m_assetBalance{nullptr};
    QLabel* m_network{nullptr};
    QLabel* m_height{nullptr};
    QLabel* m_conn{nullptr};
    QLabel* m_syncPct{nullptr};
    QLabel* m_syncStatus{nullptr};
    QVBoxLayout* m_activityList{nullptr};
    WalletModel* m_walletModel{nullptr};
};

#endif // BITCOIN_QT_DASHBOARDPAGE_H
