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
#include <qt/walletcard.h>
#include <qt/walletmodel.h>

#include <outputtype.h>

#include <QAbstractAnimation>
#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QEasingCurve>
#include <QFormLayout>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QShowEvent>
#include <QSizePolicy>
#include <QVBoxLayout>

namespace {
void FadeWidget(QWidget* widget, int duration = 240)
{
    if (!widget) return;
    auto* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    auto* anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(duration);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    QObject::connect(anim, &QPropertyAnimation::finished, widget, [widget] {
        widget->setGraphicsEffect(nullptr);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
} // namespace

ReceivePage::ReceivePage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("receivePage"));

    QVBoxLayout* page = new QVBoxLayout(this);
    page->setContentsMargins(0, 0, 0, 0);
    page->setSpacing(0);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setObjectName(QStringLiteral("pageScrollArea"));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_content = new QWidget(scroll);
    m_content->setObjectName(QStringLiteral("pageContainer"));
    QVBoxLayout* root = new QVBoxLayout(m_content);
    root->setContentsMargins(18, 18, 18, 22);
    root->setSpacing(18);

    QLabel* heading = new QLabel(tr("Receive"), m_content);
    heading->setObjectName(QStringLiteral("pageHeading"));
    root->addWidget(heading);

    WalletCard* requestCard = new WalletCard(tr("Request details"), m_content);
    QFormLayout* form = new QFormLayout();
    form->setObjectName(QStringLiteral("premiumForm"));
    form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    form->setFormAlignment(Qt::AlignTop);
    form->setHorizontalSpacing(12);
    form->setVerticalSpacing(12);

    m_label = new QLineEdit(requestCard);
    m_amount = new BitcoinAmountField(requestCard);
    m_message = new QLineEdit(requestCard);
    m_type = new QComboBox(requestCard);
    m_type->addItem(tr("Bech32 (kpepe1...)"), static_cast<int>(OutputType::BECH32));
    m_type->addItem(tr("P2SH-SegWit"), static_cast<int>(OutputType::P2SH_SEGWIT));
    m_type->addItem(tr("Legacy (M...)"), static_cast<int>(OutputType::LEGACY));
    auto polishField = [](QWidget* field) {
        field->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        field->setMinimumHeight(42);
    };
    polishField(m_label);
    polishField(m_amount);
    polishField(m_message);
    polishField(m_type);

    form->addRow(tr("Label"), m_label);
    form->addRow(tr("Amount"), m_amount);
    form->addRow(tr("Message"), m_message);
    form->addRow(tr("Address type"), m_type);
    requestCard->addBodyLayout(form);

    QPushButton* generateButton = new QPushButton(tr("Generate receiving address"), requestCard);
    generateButton->setObjectName(QStringLiteral("primaryActionButton"));
    connect(generateButton, &QPushButton::clicked, this, &ReceivePage::generate);
    requestCard->addBodyWidget(generateButton);
    root->addWidget(requestCard);

    WalletCard* resultCard = new WalletCard(tr("Receiving address"), m_content);
    m_resultCard = resultCard;
    m_address = new QLabel(tr("Generate a receiving address to display it here."), resultCard);
    m_address->setObjectName(QStringLiteral("receiveAddressValue"));
    m_address->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_address->setWordWrap(true);
    resultCard->addBodyWidget(m_address);

    m_copyButton = new QPushButton(tr("Copy address"), resultCard);
    m_copyButton->setObjectName(QStringLiteral("secondaryActionButton"));
    m_copyButton->setEnabled(false);
    connect(m_copyButton, &QPushButton::clicked, this, &ReceivePage::copyAddress);
    resultCard->addBodyWidget(m_copyButton);

    m_qr = new QRImageWidget(resultCard);
    m_qr->setObjectName(QStringLiteral("receiveQr"));
    m_qr->setVisible(false);
    resultCard->addBodyWidget(m_qr);
    root->addWidget(resultCard);

    root->addStretch(1);
    scroll->setWidget(m_content);
    page->addWidget(scroll);
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
    // Reuse the exact existing derivation path - no new key/address logic.
    const QString address = m_model->getAddressTableModel()->addRow(
        AddressTableModel::Receive, label, QString(), type);
    if (address.isEmpty()) {
        m_address->setText(tr("Could not generate a new address."));
        if (m_copyButton) m_copyButton->setEnabled(false);
        if (m_qr) m_qr->setVisible(false);
        return;
    }

    SendCoinsRecipient info(address, label, m_amount->value(), m_message->text());
    m_model->getRecentRequestsTableModel()->addNewRequest(info);

    m_address->setText(address);
    m_qr->setQR(GUIUtil::formatBitcoinURI(info), address);
    m_qr->setVisible(true);
    if (m_copyButton) m_copyButton->setEnabled(true);
    FadeWidget(m_resultCard, 220);
}

void ReceivePage::copyAddress()
{
    if (m_address && m_copyButton && m_copyButton->isEnabled()) QApplication::clipboard()->setText(m_address->text());
}

void ReceivePage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    FadeWidget(m_content);
}
