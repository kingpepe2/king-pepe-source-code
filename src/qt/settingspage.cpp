// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/settingspage.h>

#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/walletcard.h>

#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QSettings>
#include <QShowEvent>
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

void AddSettingsRow(WalletCard* card, const QString& key, QLabel* value)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setObjectName(QStringLiteral("settingsOptionRow"));
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(12);
    QLabel* k = new QLabel(key, card);
    k->setObjectName(QStringLiteral("settingsOptionKey"));
    value->setObjectName(QStringLiteral("settingsOptionValue"));
    value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    row->addWidget(k);
    row->addStretch(1);
    row->addWidget(value);
    card->addBodyLayout(row);
}
} // namespace

SettingsPage::SettingsPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("settingsPage"));
    setWindowFlag(Qt::Window);
    setWindowTitle(tr("Settings"));
    resize(440, 640);
    setMinimumSize(390, 560);
    setMaximumWidth(600);

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

    QLabel* heading = new QLabel(tr("Settings"), m_content);
    heading->setObjectName(QStringLiteral("pageHeading"));
    root->addWidget(heading);

    WalletCard* appearance = new WalletCard(tr("Appearance"), m_content);
    QHBoxLayout* themeRow = new QHBoxLayout();
    themeRow->setSpacing(12);
    QPushButton* darkButton = new QPushButton(tr("Dark"), appearance);
    QPushButton* lightButton = new QPushButton(tr("Light"), appearance);
    darkButton->setObjectName(QStringLiteral("primaryActionButton"));
    lightButton->setObjectName(QStringLiteral("secondaryActionButton"));
    connect(darkButton, &QPushButton::clicked, this, [] {
        QSettings().setValue("theme", "dark");
        GUIUtil::applyTheme("dark");
    });
    connect(lightButton, &QPushButton::clicked, this, [] {
        QSettings().setValue("theme", "light");
        GUIUtil::applyTheme("light");
    });
    themeRow->addWidget(darkButton);
    themeRow->addWidget(lightButton);
    appearance->addBodyLayout(themeRow);
    root->addWidget(appearance);

    WalletCard* walletOptions = new WalletCard(tr("Wallet options"), m_content);
    m_tray = new QLabel(walletOptions);
    m_minimize = new QLabel(walletOptions);
    m_close = new QLabel(walletOptions);
    m_coincontrol = new QLabel(walletOptions);
    AddSettingsRow(walletOptions, tr("Show tray icon"), m_tray);
    AddSettingsRow(walletOptions, tr("Minimize to tray"), m_minimize);
    AddSettingsRow(walletOptions, tr("Minimize on close"), m_close);
    AddSettingsRow(walletOptions, tr("Coin control features"), m_coincontrol);
    root->addWidget(walletOptions);

    WalletCard* advancedCard = new WalletCard(tr("Advanced"), m_content);
    QPushButton* advanced = new QPushButton(tr("Advanced settings..."), advancedCard);
    advanced->setObjectName(QStringLiteral("primaryActionButton"));
    connect(advanced, &QPushButton::clicked, this, &SettingsPage::advancedRequested);
    advancedCard->addBodyWidget(advanced);
    root->addWidget(advancedCard);

    root->addStretch(1);
    scroll->setWidget(m_content);
    page->addWidget(scroll);
}

void SettingsPage::setModel(OptionsModel* model)
{
    m_model = model;
    refresh();
}

void SettingsPage::refresh()
{
    if (!m_model) return;
    const QString on = tr("On");
    const QString off = tr("Off");
    m_tray->setText(m_model->getShowTrayIcon() ? on : off);
    m_minimize->setText(m_model->getMinimizeToTray() ? on : off);
    m_close->setText(m_model->getMinimizeOnClose() ? on : off);
    m_coincontrol->setText(m_model->getCoinControlFeatures() ? on : off);
}

void SettingsPage::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    FadeWidget(m_content);
}
