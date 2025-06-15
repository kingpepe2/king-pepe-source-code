// Copyright (c) 2011-2020 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KINGPEPE_QT_KINGPEPEADDRESSVALIDATOR_H
#define KINGPEPE_QT_KINGPEPEADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class KingpepeAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit KingpepeAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

/** Kingpepe address widget validator, checks for a valid kingpepe address.
 */
class KingpepeAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit KingpepeAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const override;
};

#endif // KINGPEPE_QT_KINGPEPEADDRESSVALIDATOR_H
