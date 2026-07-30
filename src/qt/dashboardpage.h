// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_DASHBOARDPAGE_H
#define BITCOIN_QT_DASHBOARDPAGE_H

#include <QWidget>

class StatCard;
class WalletCard;

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

/**
 * KingPepe modern dashboard page.
 *
 * Composes the reusable card components into a responsive overview. It holds no
 * wallet state: the host feeds it formatted strings from the existing
 * WalletModel/ClientModel via the setters below, and it emits sendRequested()/
 * receiveRequested() for the host to route to the existing pages. This keeps the
 * backend untouched while the presentation is fully custom.
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
    void setSyncProgress(const QString& progress);

Q_SIGNALS:
    void sendRequested();
    void receiveRequested();

private:
    WalletCard* m_balanceCard{nullptr};
    QLabel* m_total{nullptr};
    QLabel* m_available{nullptr};
    QLabel* m_pending{nullptr};
    QLabel* m_immature{nullptr};
    StatCard* m_heightCard{nullptr};
    StatCard* m_connCard{nullptr};
    StatCard* m_syncCard{nullptr};
};

#endif // BITCOIN_QT_DASHBOARDPAGE_H
