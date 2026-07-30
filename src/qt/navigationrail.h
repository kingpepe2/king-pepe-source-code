// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_NAVIGATIONRAIL_H
#define BITCOIN_QT_NAVIGATIONRAIL_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QButtonGroup;
class QVBoxLayout;
QT_END_NAMESPACE

/**
 * KingPepe modern navigation sidebar.
 *
 * A vertical, exclusive list of checkable nav items with an active-state.
 * Presentational only: it emits selected(index) and leaves page switching to
 * the host (e.g. a QStackedWidget). Styled via QSS ("navigationRail"/"navItem").
 */
class NavigationRail : public QWidget
{
    Q_OBJECT

public:
    explicit NavigationRail(QWidget* parent = nullptr);

    //! Append a navigation item; returns its index.
    int addItem(const QString& text);

    //! Programmatically select an item (does not re-emit selected()).
    void setCurrentIndex(int index);

Q_SIGNALS:
    void selected(int index);

private:
    QVBoxLayout* m_layout{nullptr};
    QButtonGroup* m_group{nullptr};
};

#endif // BITCOIN_QT_NAVIGATIONRAIL_H
