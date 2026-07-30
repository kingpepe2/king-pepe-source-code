// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/walletcard.h>

#include <QLabel>
#include <QLayout>
#include <QVBoxLayout>
#include <QWidget>

WalletCard::WalletCard(const QString& title, QWidget* parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("walletCard"));

    m_body = new QVBoxLayout(this);
    m_body->setContentsMargins(18, 18, 18, 18);
    m_body->setSpacing(12);

    m_title = new QLabel(title, this);
    m_title->setObjectName(QStringLiteral("walletCardTitle"));
    m_title->setVisible(!title.isEmpty());
    m_body->addWidget(m_title);
}

void WalletCard::setTitle(const QString& title)
{
    m_title->setText(title);
    m_title->setVisible(!title.isEmpty());
}

void WalletCard::addBodyWidget(QWidget* widget)
{
    m_body->addWidget(widget);
}

void WalletCard::addBodyLayout(QLayout* layout)
{
    m_body->addLayout(layout);
}
