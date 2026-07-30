// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/addressbookwidget.h>

#include <qt/addresstablemodel.h>
#include <qt/editaddressdialog.h>
#include <qt/walletmodel.h>

#include <QAbstractItemModel>
#include <QApplication>
#include <QClipboard>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

AddressBookWidget::AddressBookWidget(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("addressBookWidget"));
    setWindowFlag(Qt::Window);
    setWindowTitle(tr("Address Book"));
    resize(560, 480);

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(20, 20, 20, 20);
    root->setSpacing(12);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* heading = new QLabel(tr("Address Book"), this);
    heading->setObjectName(QStringLiteral("pageHeading"));
    QPushButton* newButton = new QPushButton(tr("New sending address"), this);
    newButton->setObjectName(QStringLiteral("dashSend"));
    connect(newButton, &QPushButton::clicked, this, &AddressBookWidget::newAddress);
    header->addWidget(heading);
    header->addStretch(1);
    header->addWidget(newButton);
    root->addLayout(header);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    QWidget* container = new QWidget(scroll);
    m_list = new QVBoxLayout(container);
    m_list->setContentsMargins(0, 0, 0, 0);
    m_list->setSpacing(8);
    m_list->addStretch(1);
    scroll->setWidget(container);
    root->addWidget(scroll);
}

void AddressBookWidget::setModel(WalletModel* model)
{
    m_walletModel = model;
    if (!model) return;
    m_model = model->getAddressTableModel();
    if (!m_model) return;

    connect(m_model, &QAbstractItemModel::modelReset, this, &AddressBookWidget::rebuild);
    connect(m_model, &QAbstractItemModel::rowsInserted, this, &AddressBookWidget::rebuild);
    connect(m_model, &QAbstractItemModel::rowsRemoved, this, &AddressBookWidget::rebuild);
    connect(m_model, &QAbstractItemModel::dataChanged, this, &AddressBookWidget::rebuild);
    connect(m_model, &QAbstractItemModel::layoutChanged, this, &AddressBookWidget::rebuild);

    rebuild();
}

void AddressBookWidget::rebuild()
{
    if (!m_model || !m_list) return;

    while (m_list->count() > 1) {
        QLayoutItem* item = m_list->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    const int rows = m_model->rowCount(QModelIndex());
    for (int r = 0; r < rows; ++r) {
        const QString label = m_model->index(r, AddressTableModel::Label).data().toString();
        const QString address = m_model->index(r, AddressTableModel::Address).data().toString();
        const QString type = m_model->index(r, 0).data(AddressTableModel::TypeRole).toString();

        QFrame* row = new QFrame();
        row->setObjectName(QStringLiteral("txRow"));
        QHBoxLayout* h = new QHBoxLayout(row);
        h->setContentsMargins(14, 10, 14, 10);
        h->setSpacing(12);

        QLabel* who = new QLabel(label.isEmpty() ? tr("(no label)") : label, row);
        who->setObjectName(QStringLiteral("txWho"));
        QLabel* addr = new QLabel(address, row);
        addr->setObjectName(QStringLiteral("txWhen"));
        h->addWidget(who);
        h->addWidget(addr);
        h->addStretch(1);

        QPushButton* copyButton = new QPushButton(tr("Copy"), row);
        connect(copyButton, &QPushButton::clicked, this, [address] { QApplication::clipboard()->setText(address); });
        h->addWidget(copyButton);

        if (type == AddressTableModel::Send) {
            QPushButton* deleteButton = new QPushButton(tr("Delete"), row);
            connect(deleteButton, &QPushButton::clicked, this, [this, r] {
                if (m_model) m_model->removeRows(r, 1);
            });
            h->addWidget(deleteButton);
        }

        m_list->insertWidget(m_list->count() - 1, row);
    }
}

void AddressBookWidget::newAddress()
{
    if (!m_model) return;
    EditAddressDialog dlg(EditAddressDialog::NewSendingAddress, this);
    dlg.setModel(m_model);
    dlg.exec();
}
