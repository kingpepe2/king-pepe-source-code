// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/receivepage.h>

#include <qt/addresstablemodel.h>
#include <qt/bitcoinamountfield.h>
#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/qrimagewidget.h>
#include <qt/recentrequeststablemodel.h>
#include <qt/sendcoinsrecipient.h>
#include <qt/walletmodel.h>

#include <outputtype.h>

#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

ReceivePage::ReceivePage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("receivePage"));

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(14);

    QLabel* heading = new QLabel(tr("Receive"), this);
    heading->setObjectName(QStringLiteral("pageHeading"));
    root->addWidget(heading);

    QFormLayout* form = new QFormLayout();
    m_label = new QLineEdit(this);
    m_amount = new BitcoinAmountField(this);
    m_message = new QLineEdit(this);
    m_type = new QComboBox(this);
    m_type->addItem(tr("Bech32 (kpepe1…)"), static_cast<int>(OutputType::BECH32));
    m_type->addItem(tr("P2SH-SegWit"), static_cast<int>(OutputType::P2SH_SEGWIT));
    m_type->addItem(tr("Legacy (M…)"), static_cast<int>(OutputType::LEGACY));
    form->addRow(tr("Label"), m_label);
    form->addRow(tr("Amount"), m_amount);
    form->addRow(tr("Message"), m_message);
    form->addRow(tr("Address type"), m_type);
    root->addLayout(form);

    QPushButton* generateButton = new QPushButton(tr("Generate receiving address"), this);
    generateButton->setObjectName(QStringLiteral("dashSend"));
    connect(generateButton, &QPushButton::clicked, this, &ReceivePage::generate);
    root->addWidget(generateButton);

    m_address = new QLabel(this);
    m_address->setObjectName(QStringLiteral("txWho"));
    m_address->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_address->setWordWrap(true);
    root->addWidget(m_address);

    QPushButton* copyButton = new QPushButton(tr("Copy address"), this);
    connect(copyButton, &QPushButton::clicked, this, &ReceivePage::copyAddress);
    root->addWidget(copyButton);

    m_qr = new QRImageWidget(this);
    root->addWidget(m_qr);

    root->addStretch(1);
}

void ReceivePage::setModel(WalletModel* model)
{
    m_model = model;
    if (model && model->getOptionsModel()) {
        m_amount->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
    }
}

void ReceivePage::generate()
{
    if (!m_model || !m_model->getAddressTableModel() || !m_model->getRecentRequestsTableModel()) return;

    const OutputType type = static_cast<OutputType>(m_type->currentData().toInt());
    const QString label = m_label->text();
    // Reuse the exact existing derivation path — no new key/address logic.
    const QString address = m_model->getAddressTableModel()->addRow(
        AddressTableModel::Receive, label, QString(), type);
    if (address.isEmpty()) {
        m_address->setText(tr("Could not generate a new address."));
        return;
    }

    SendCoinsRecipient info(address, label, m_amount->value(), m_message->text());
    m_model->getRecentRequestsTableModel()->addNewRequest(info);

    m_address->setText(address);
    m_qr->setQR(GUIUtil::formatBitcoinURI(info), address);
}

void ReceivePage::copyAddress()
{
    if (m_address) QApplication::clipboard()->setText(m_address->text());
}
