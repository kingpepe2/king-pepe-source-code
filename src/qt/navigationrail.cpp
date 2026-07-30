// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/navigationrail.h>

#include <QAbstractButton>
#include <QButtonGroup>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>

NavigationRail::NavigationRail(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("navigationRail"));

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(10, 14, 10, 14);
    m_layout->setSpacing(4);

    m_group = new QButtonGroup(this);
    m_group->setExclusive(true);
    connect(m_group, &QButtonGroup::idClicked, this, &NavigationRail::selected);

    // Trailing stretch so items stack from the top.
    m_layout->addStretch(1);
}

int NavigationRail::addItem(const QString& text)
{
    QToolButton* button = new QToolButton(this);
    button->setText(text);
    button->setObjectName(QStringLiteral("navItem"));
    button->setCheckable(true);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    const int index = static_cast<int>(m_group->buttons().size());
    m_group->addButton(button, index);

    // Insert above the trailing stretch.
    m_layout->insertWidget(m_layout->count() - 1, button);

    if (index == 0) button->setChecked(true);
    return index;
}

void NavigationRail::setCurrentIndex(int index)
{
    if (QAbstractButton* button = m_group->button(index)) {
        button->setChecked(true);
    }
}
