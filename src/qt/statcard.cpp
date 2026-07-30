// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/statcard.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

StatCard::StatCard(const QString& caption, QWidget* parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("statCard"));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(4);

    m_caption = new QLabel(caption, this);
    m_caption->setObjectName(QStringLiteral("statCaption"));

    m_value = new QLabel(QStringLiteral("—"), this);
    m_value->setObjectName(QStringLiteral("statValue"));

    m_subtext = new QLabel(this);
    m_subtext->setObjectName(QStringLiteral("statSubtext"));
    m_subtext->setVisible(false);

    layout->addWidget(m_caption);
    layout->addWidget(m_value);
    layout->addWidget(m_subtext);
    layout->addStretch(1);
}

void StatCard::setCaption(const QString& caption)
{
    m_caption->setText(caption);
}

void StatCard::setValue(const QString& value)
{
    m_value->setText(value);
}

void StatCard::setSubtext(const QString& subtext)
{
    m_subtext->setText(subtext);
    m_subtext->setVisible(!subtext.isEmpty());
}
