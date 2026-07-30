// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/dashboardpage.h>

#include <qt/statcard.h>
#include <qt/walletcard.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

DashboardPage::DashboardPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("dashboardPage"));

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(16);

    // --- Balance card ---
    m_balanceCard = new WalletCard(tr("Total balance"), this);

    m_total = new QLabel(QStringLiteral("—"), this);
    m_total->setObjectName(QStringLiteral("dashTotal"));
    m_balanceCard->addBodyWidget(m_total);

    QHBoxLayout* subRow = new QHBoxLayout();
    subRow->setSpacing(24);
    m_available = new QLabel(this);
    m_pending = new QLabel(this);
    m_immature = new QLabel(this);
    subRow->addWidget(m_available);
    subRow->addWidget(m_pending);
    subRow->addWidget(m_immature);
    subRow->addStretch(1);
    m_balanceCard->addBodyLayout(subRow);

    QHBoxLayout* actions = new QHBoxLayout();
    QPushButton* sendButton = new QPushButton(tr("Send"), this);
    sendButton->setObjectName(QStringLiteral("dashSend"));
    sendButton->setDefault(true);
    QPushButton* receiveButton = new QPushButton(tr("Receive"), this);
    receiveButton->setObjectName(QStringLiteral("dashReceive"));
    connect(sendButton, &QPushButton::clicked, this, &DashboardPage::sendRequested);
    connect(receiveButton, &QPushButton::clicked, this, &DashboardPage::receiveRequested);
    actions->addWidget(sendButton);
    actions->addWidget(receiveButton);
    actions->addStretch(1);
    m_balanceCard->addBodyLayout(actions);

    root->addWidget(m_balanceCard);

    // --- Stat row ---
    QHBoxLayout* stats = new QHBoxLayout();
    stats->setSpacing(16);
    m_heightCard = new StatCard(tr("Block height"), this);
    m_connCard = new StatCard(tr("Connections"), this);
    m_syncCard = new StatCard(tr("Sync"), this);
    stats->addWidget(m_heightCard);
    stats->addWidget(m_connCard);
    stats->addWidget(m_syncCard);
    root->addLayout(stats);

    root->addStretch(1);
}

void DashboardPage::setBalance(const QString& total, const QString& available,
                               const QString& pending, const QString& immature)
{
    m_total->setText(total);
    m_available->setText(tr("Available: %1").arg(available));
    m_pending->setText(tr("Pending: %1").arg(pending));
    m_immature->setText(tr("Immature: %1").arg(immature));
}

void DashboardPage::setBlockHeight(const QString& height) { m_heightCard->setValue(height); }
void DashboardPage::setConnections(const QString& peers) { m_connCard->setValue(peers); }
void DashboardPage::setSyncProgress(const QString& progress) { m_syncCard->setValue(progress); }
