// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_STATCARD_H
#define BITCOIN_QT_STATCARD_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QLabel;
class QWidget;
QT_END_NAMESPACE

/**
 * KingPepe dashboard stat card: a caption, a prominent value, and an optional
 * subtext. Presentational only; styled via QSS ("statCard"/"statCaption"/
 * "statValue"/"statSubtext"). Feed it strings from the existing wallet/client
 * models — it holds no state of its own.
 */
class StatCard : public QFrame
{
    Q_OBJECT

public:
    explicit StatCard(const QString& caption = QString(), QWidget* parent = nullptr);

    void setCaption(const QString& caption);
    void setValue(const QString& value);
    void setSubtext(const QString& subtext);

private:
    QLabel* m_caption{nullptr};
    QLabel* m_value{nullptr};
    QLabel* m_subtext{nullptr};
};

#endif // BITCOIN_QT_STATCARD_H
