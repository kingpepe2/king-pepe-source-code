// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/compactheader.h>

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QSizePolicy>
#include <QToolButton>

CompactHeader::CompactHeader(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("compactHeader"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 8, 12, 8);
    layout->setSpacing(8);

    // KingPepe logo (own brand asset from resources).
    QLabel* logo = new QLabel(this);
    logo->setObjectName(QStringLiteral("compactLogo"));
    QPixmap px(QStringLiteral(":/icons/bitcoin"));
    if (!px.isNull()) {
        logo->setPixmap(px.scaled(26, 26, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    m_name = new QLabel(tr("KingPepe"), this);
    m_name->setObjectName(QStringLiteral("compactWalletName"));

    m_network = new QLabel(this);
    m_network->setObjectName(QStringLiteral("compactNetwork"));

    m_lock = new QLabel(this);
    m_lock->setObjectName(QStringLiteral("compactLock"));

    QToolButton* settings = new QToolButton(this);
    settings->setObjectName(QStringLiteral("compactSettingsButton"));
    settings->setText(QStringLiteral("⚙")); // gear
    settings->setToolTip(tr("Settings"));
    connect(settings, &QToolButton::clicked, this, &CompactHeader::settingsRequested);

    layout->addWidget(logo);
    layout->addWidget(m_name);
    layout->addStretch(1);
    layout->addWidget(m_network);
    layout->addWidget(m_lock);
    layout->addWidget(settings);
}

void CompactHeader::setWalletName(const QString& name)
{
    m_name->setText(name.isEmpty() ? tr("KingPepe") : name);
}

void CompactHeader::setNetwork(const QString& network)
{
    m_network->setText(network);
}

void CompactHeader::setLockStatus(const QString& status)
{
    m_lock->setText(status);
}
