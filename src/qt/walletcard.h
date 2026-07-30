// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_WALLETCARD_H
#define BITCOIN_QT_WALLETCARD_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QLabel;
class QLayout;
class QVBoxLayout;
class QWidget;
QT_END_NAMESPACE

/**
 * KingPepe reusable "card" component.
 *
 * A rounded, elevated container with an optional title, used as the building
 * block for the modern dashboard and panels. Purely presentational: it owns no
 * wallet/consensus state and is styled entirely via QSS using the object names
 * "walletCard" (the frame) and "walletCardTitle" (the heading).
 */
class WalletCard : public QFrame
{
    Q_OBJECT

public:
    explicit WalletCard(const QString& title = QString(), QWidget* parent = nullptr);

    //! Set (or clear) the card heading. An empty title hides the heading.
    void setTitle(const QString& title);

    //! Add a widget to the card body.
    void addBodyWidget(QWidget* widget);

    //! Add a nested layout to the card body.
    void addBodyLayout(QLayout* layout);

private:
    QLabel* m_title{nullptr};
    QVBoxLayout* m_body{nullptr};
};

#endif // BITCOIN_QT_WALLETCARD_H
