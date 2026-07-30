// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_COMPACTHEADER_H
#define BITCOIN_QT_COMPACTHEADER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

/**
 * KingPepe compact top header: logo, wallet name, network indicator, lock status,
 * and a settings button. Presentational only; the host feeds it strings and
 * handles settingsRequested(). Styled via QSS.
 */
class CompactHeader : public QWidget
{
    Q_OBJECT

public:
    explicit CompactHeader(QWidget* parent = nullptr);

    void setWalletName(const QString& name);
    void setNetwork(const QString& network);
    void setLockStatus(const QString& status);
    void setConnections(int count);
    void setSyncActive(bool active);

Q_SIGNALS:
    void settingsRequested();

private:
    QLabel* m_name{nullptr};
    QLabel* m_network{nullptr};
    QLabel* m_lock{nullptr};
    QWidget* m_sync_indicator{nullptr};
    QWidget* m_connection_indicator{nullptr};
    QLabel* m_connections{nullptr};
};

#endif // BITCOIN_QT_COMPACTHEADER_H
