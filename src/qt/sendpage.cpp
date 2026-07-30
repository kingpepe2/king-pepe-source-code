// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/sendpage.h>

#include <QAbstractAnimation>
#include <QBoxLayout>
#include <QEasingCurve>
#include <QFrame>
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QShowEvent>
#include <QSizePolicy>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

namespace {
void FadePage(QWidget* widget)
{
    if (!widget) return;
    auto* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    auto* anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(240);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    QObject::connect(anim, &QPropertyAnimation::finished, widget, [widget] {
        widget->setGraphicsEffect(nullptr);
    });
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
} // namespace

SendPage::SendPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("sendPage"));

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
    m_root = new QVBoxLayout(m_content);
    m_root->setContentsMargins(18, 18, 18, 22);
    m_root->setSpacing(18);

    QLabel* heading = new QLabel(tr("Send"), this);
    heading->setObjectName(QStringLiteral("pageHeading"));
    m_root->addWidget(heading);

    QFrame* host = new QFrame(m_content);
    host->setObjectName(QStringLiteral("sendHostCard"));
    m_host_layout = new QVBoxLayout(host);
    m_host_layout->setContentsMargins(0, 0, 0, 0);
    m_host_layout->setSpacing(0);
    m_root->addWidget(host);
    m_root->addStretch(1);

    scroll->setWidget(m_content);
    page->addWidget(scroll);
}

void SendPage::setSendWidget(QWidget* sendWidget)
{
    // Reparents the proven send widget into this shell; its logic/wiring is untouched.
    if (!sendWidget || !m_host_layout) return;
    m_sendWidget = sendWidget;
    sendWidget->setObjectName(QStringLiteral("embeddedSendDialog"));
    sendWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_host_layout->addWidget(sendWidget);
    QTimer::singleShot(0, this, &SendPage::tuneSendWidget);
    if (QPushButton* addButton = sendWidget->findChild<QPushButton*>(QStringLiteral("addButton"))) {
        connect(addButton, &QPushButton::clicked, this, [this] {
            QTimer::singleShot(0, this, &SendPage::tuneSendWidget);
        });
    }
}

void SendPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    tuneSendWidget();
    FadePage(m_content);
}

void SendPage::tuneSendWidget()
{
    if (!m_sendWidget) return;
    m_sendWidget->setMinimumWidth(0);

    if (QScrollArea* recipients = m_sendWidget->findChild<QScrollArea*>(QStringLiteral("scrollArea"))) {
        recipients->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        recipients->setWidgetResizable(true);
        recipients->setMinimumHeight(330);
        if (recipients->verticalScrollBar()) {
            recipients->verticalScrollBar()->setValue(0);
        }
        if (QWidget* contents = recipients->widget()) {
            const int target_width = qMax(280, recipients->viewport()->width());
            contents->setMinimumWidth(0);
            contents->setFixedWidth(target_width);
        }
    }

    for (QWidget* entry : m_sendWidget->findChildren<QWidget*>(QStringLiteral("SendCoinsEntry"))) {
        entry->setMinimumWidth(0);
        entry->setMinimumHeight(292);
        entry->setMaximumWidth(QWIDGETSIZE_MAX);
        entry->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }

    for (QGridLayout* grid : m_sendWidget->findChildren<QGridLayout*>(QStringLiteral("gridLayout"))) {
        grid->setHorizontalSpacing(8);
        grid->setVerticalSpacing(10);
        grid->setColumnMinimumWidth(0, 56);
        grid->setColumnStretch(1, 1);
    }

    for (QBoxLayout* amountLayout : m_sendWidget->findChildren<QBoxLayout*>(QStringLiteral("horizontalLayoutAmount"))) {
        amountLayout->setDirection(QBoxLayout::TopToBottom);
        amountLayout->setSpacing(8);
    }

    for (const QString& layoutName : {QStringLiteral("horizontalLayoutFee1"),
                                      QStringLiteral("horizontalLayoutSmartFee"),
                                      QStringLiteral("horizontalLayoutFee12"),
                                      QStringLiteral("horizontalLayout")}) {
        for (QBoxLayout* feeLayout : m_sendWidget->findChildren<QBoxLayout*>(layoutName)) {
            feeLayout->setDirection(QBoxLayout::TopToBottom);
            feeLayout->setSpacing(8);
        }
    }

    for (QLabel* label : m_sendWidget->findChildren<QLabel*>()) {
        label->setWordWrap(true);
        label->setMinimumWidth(0);
    }

    for (QToolButton* toolButton : m_sendWidget->findChildren<QToolButton*>()) {
        toolButton->setFixedSize(34, 42);
    }

    for (QPushButton* button : m_sendWidget->findChildren<QPushButton*>()) {
        if (button->objectName() == QLatin1String("useAvailableBalanceButton")) {
            button->setText(tr("Max"));
            button->setFixedWidth(92);
            button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            button->setMinimumHeight(42);
            continue;
        }
        button->setMinimumHeight(42);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    if (QLineEdit* payTo = m_sendWidget->findChild<QLineEdit*>(QStringLiteral("payTo"))) {
        payTo->setPlaceholderText(tr("Enter KingPepe address"));
    }
    if (QLineEdit* label = m_sendWidget->findChild<QLineEdit*>(QStringLiteral("addAsLabel"))) {
        label->setPlaceholderText(tr("Optional label"));
    }
}
