// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_SETTINGSPAGE_H
#define BITCOIN_QT_SETTINGSPAGE_H

#include <QWidget>

class OptionsModel;

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

/**
 * KingPepe modern settings window (custom widget).
 *
 * Provides the live theme switch (safe, QSettings-backed) and a read-only
 * summary of key options, and hands off to the proven OptionsDialog for all
 * editable options via advancedRequested(). It performs no risky option writes,
 * so settings persistence is never jeopardised.
 */
class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(QWidget* parent = nullptr);

    void setModel(OptionsModel* model);

Q_SIGNALS:
    void advancedRequested();

private:
    void refresh();

    OptionsModel* m_model{nullptr};
    QLabel* m_tray{nullptr};
    QLabel* m_minimize{nullptr};
    QLabel* m_close{nullptr};
    QLabel* m_coincontrol{nullptr};
};

#endif // BITCOIN_QT_SETTINGSPAGE_H
