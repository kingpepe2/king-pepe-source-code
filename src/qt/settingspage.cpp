// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/settingspage.h>

#include <qt/guiutil.h>
#include <qt/optionsmodel.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>

SettingsPage::SettingsPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("settingsPage"));
    setWindowFlag(Qt::Window);
    setWindowTitle(tr("Settings"));
    resize(520, 460);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(14);

    QLabel* heading = new QLabel(tr("Settings"), this);
    heading->setObjectName(QStringLiteral("pageHeading"));
    root->addWidget(heading);

    QLabel* themeLabel = new QLabel(tr("Theme"), this);
    themeLabel->setObjectName(QStringLiteral("statCaption"));
    root->addWidget(themeLabel);

    QHBoxLayout* themeRow = new QHBoxLayout();
    QPushButton* darkButton = new QPushButton(tr("Dark"), this);
    QPushButton* lightButton = new QPushButton(tr("Light"), this);
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
    themeRow->addStretch(1);
    root->addLayout(themeRow);

    m_tray = new QLabel(this);
    m_minimize = new QLabel(this);
    m_close = new QLabel(this);
    m_coincontrol = new QLabel(this);
    for (QLabel* label : {m_tray, m_minimize, m_close, m_coincontrol}) {
        label->setObjectName(QStringLiteral("txWhen"));
        root->addWidget(label);
    }

    root->addStretch(1);

    QPushButton* advanced = new QPushButton(tr("Advanced settings…"), this);
    advanced->setObjectName(QStringLiteral("dashSend"));
    connect(advanced, &QPushButton::clicked, this, &SettingsPage::advancedRequested);
    root->addWidget(advanced);
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
    m_tray->setText(tr("Show tray icon: %1").arg(m_model->getShowTrayIcon() ? on : off));
    m_minimize->setText(tr("Minimize to tray: %1").arg(m_model->getMinimizeToTray() ? on : off));
    m_close->setText(tr("Minimize on close: %1").arg(m_model->getMinimizeOnClose() ? on : off));
    m_coincontrol->setText(tr("Coin control features: %1").arg(m_model->getCoinControlFeatures() ? on : off));
}
