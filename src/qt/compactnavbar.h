// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_COMPACTNAVBAR_H
#define BITCOIN_QT_COMPACTNAVBAR_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QHBoxLayout;
QT_END_NAMESPACE

/**
 * KingPepe compact bottom navigation bar (Phantom-style layout reference only).
 *
 * A horizontal row of exclusive, checkable items (Home/Send/Receive/Activity/
 * Settings). Presentational: it emits navigated(index) and leaves page switching
 * to the host. Styled via QSS ("compactNavBar"/"compactNavItem").
 */
class CompactNavBar : public QWidget
{
    Q_OBJECT

public:
    explicit CompactNavBar(QWidget* parent = nullptr);

    //! Append a navigation item; returns its index.
    int addItem(const QString& text);

    //! Programmatically mark an item active (does not re-emit navigated()).
    void setActive(int index);

Q_SIGNALS:
    void navigated(int index);

private:
    QHBoxLayout* m_layout{nullptr};
    QButtonGroup* m_group{nullptr};
};

#endif // BITCOIN_QT_COMPACTNAVBAR_H
