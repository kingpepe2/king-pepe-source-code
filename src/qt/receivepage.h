// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_RECEIVEPAGE_H
#define BITCOIN_QT_RECEIVEPAGE_H

#include <QWidget>

class BitcoinAmountField;
class QRImageWidget;
class WalletModel;

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QShowEvent;
class QWidget;
QT_END_NAMESPACE

/**
 * KingPepe modern Receive screen (custom widget).
 *
 * Generates receiving addresses by reusing the existing AddressTableModel::addRow
 * derivation path, renders the QR via the existing QRImageWidget, and records a
 * payment request via RecentRequestsTableModel — no address derivation or wallet
 * backend behaviour is changed here.
 */
class ReceivePage : public QWidget
{
    Q_OBJECT

public:
    explicit ReceivePage(QWidget* parent = nullptr);

    void setModel(WalletModel* model);

private Q_SLOTS:
    void generate();
    void copyAddress();

private:
    WalletModel* m_model{nullptr};
    QLineEdit* m_label{nullptr};
    BitcoinAmountField* m_amount{nullptr};
    QLineEdit* m_message{nullptr};
    QComboBox* m_type{nullptr};
    QLabel* m_address{nullptr};
    QRImageWidget* m_qr{nullptr};
    QPushButton* m_copyButton{nullptr};
    QWidget* m_content{nullptr};
    QWidget* m_resultCard{nullptr};

protected:
    void showEvent(QShowEvent* event) override;
};

#endif // BITCOIN_QT_RECEIVEPAGE_H
