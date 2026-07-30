// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_SENDPAGE_H
#define BITCOIN_QT_SENDPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

/**
 * KingPepe modern Send shell (custom widget).
 *
 * Purely presentational: it hosts the existing, proven SendCoinsDialog inside a
 * modern frame (heading + spacing + theming). It contains NO transaction logic
 * of its own — fee selection, coin control, PSBT, multi-recipient, subtract-fee,
 * signing, broadcasting and the confirmation dialog all remain in the hosted
 * SendCoinsDialog, untouched. Every satoshi still flows through the proven path.
 */
class SendPage : public QWidget
{
    Q_OBJECT

public:
    explicit SendPage(QWidget* parent = nullptr);

    //! Host the existing send widget (SendCoinsDialog); reparents it into this shell.
    void setSendWidget(QWidget* sendWidget);

private:
    QVBoxLayout* m_root{nullptr};
};

#endif // BITCOIN_QT_SENDPAGE_H
