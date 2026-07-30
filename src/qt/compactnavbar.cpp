// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/compactnavbar.h>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QToolButton>

CompactNavBar::CompactNavBar(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("compactNavBar"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(6, 4, 6, 4);
    m_layout->setSpacing(2);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);
    connect(m_group, &QButtonGroup::idClicked, this, &CompactNavBar::navigated);
}

int CompactNavBar::addItem(const QString& text)
{
    QToolButton* button = new QToolButton(this);
    button->setText(text);
    button->setObjectName(QStringLiteral("compactNavItem"));
    button->setCheckable(true);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    const int index = static_cast<int>(m_group->buttons().size());
    m_group->addButton(button, index);
    m_layout->addWidget(button);
    if (index == 0) button->setChecked(true);
    return index;
}

void CompactNavBar::setActive(int index)
{
    if (QAbstractButton* button = m_group->button(index)) {
        button->setChecked(true);
    }
}
