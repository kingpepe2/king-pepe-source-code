// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_ADDRESSBOOKWIDGET_H
#define BITCOIN_QT_ADDRESSBOOKWIDGET_H

#include <QWidget>

class AddressTableModel;
class WalletModel;

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

/**
 * KingPepe modern address book (custom widget window).
 *
 * Presents the existing AddressTableModel as modern rows with copy/delete, and
 * reuses the proven EditAddressDialog for adding entries. It only reads/edits
 * address-book labels via the model — no wallet keys, consensus, or funds are
 * touched.
 */
class AddressBookWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AddressBookWidget(QWidget* parent = nullptr);

    void setModel(WalletModel* model);

private Q_SLOTS:
    void rebuild();
    void newAddress();

private:
    WalletModel* m_walletModel{nullptr};
    AddressTableModel* m_model{nullptr};
    QVBoxLayout* m_list{nullptr};
};

#endif // BITCOIN_QT_ADDRESSBOOKWIDGET_H
