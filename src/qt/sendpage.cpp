// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/sendpage.h>

#include <QLabel>
#include <QVBoxLayout>

SendPage::SendPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("sendPage"));

    m_root = new QVBoxLayout(this);
    m_root->setContentsMargins(20, 20, 20, 20);
    m_root->setSpacing(12);

    QLabel* heading = new QLabel(tr("Send"), this);
    heading->setObjectName(QStringLiteral("pageHeading"));
    m_root->addWidget(heading);
}

void SendPage::setSendWidget(QWidget* sendWidget)
{
    // Reparents the proven send widget into this shell; its logic/wiring is untouched.
    if (sendWidget) m_root->addWidget(sendWidget);
}
