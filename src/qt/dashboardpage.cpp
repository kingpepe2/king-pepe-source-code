// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/dashboardpage.h>

#include <qt/transactiontablemodel.h>
#include <qt/walletcard.h>
#include <qt/walletmodel.h>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

#include <QAbstractAnimation>
#include <QAbstractItemModel>
#include <QEasingCurve>
#include <QDateTime>
#include <QElapsedTimer>
#include <QFrame>
#include <QGridLayout>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QLayoutItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QTimer>
#include <QScrollArea>
#include <QSize>
#include <QSizePolicy>
#include <QStyle>
#include <QVariantAnimation>
#include <QVBoxLayout>

namespace {
class AnimatedHeroCard final : public QFrame
{
public:
    explicit AnimatedHeroCard(QWidget* parent = nullptr)
        : QFrame(parent)
    {
        setObjectName(QStringLiteral("heroCard"));
        setAttribute(Qt::WA_TranslucentBackground);
        setMinimumHeight(238);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        m_clock.start();

        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this] { update(); });
        timer->start(45);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const QRectF r = rect().adjusted(1.5, 1.5, -1.5, -1.5);
        const double seconds = m_clock.elapsed() / 1000.0;
        const double pulse = (std::sin(seconds * 2.1) + 1.0) * 0.5;
        const qreal radius = 22.0;

        p.setPen(Qt::NoPen);
        for (int i = 0; i < 9; ++i) {
            QColor shadow(0, 0, 0, 34 - i * 2);
            p.setBrush(shadow);
            const QRectF shadow_rect = r.adjusted(-i * 0.5, -i * 0.2, i * 0.5, i * 0.9).translated(0, 7 + i * 0.45);
            p.drawRoundedRect(shadow_rect, radius + i * 0.3, radius + i * 0.3);
        }

        QPainterPath card_path;
        card_path.addRoundedRect(r, radius, radius);

        QRadialGradient glow(r.left() + r.width() * 0.39, r.top() + r.height() * 0.46,
                             r.width() * (0.58 + pulse * 0.13));
        glow.setColorAt(0.0, QColor(34, 197, 94, 74 + static_cast<int>(pulse * 30)));
        glow.setColorAt(0.48, QColor(34, 197, 94, 22));
        glow.setColorAt(1.0, QColor(14, 17, 23, 0));
        p.fillPath(card_path, glow);

        QLinearGradient glass(r.topLeft(), r.bottomRight());
        glass.setColorAt(0.0, QColor(22, 27, 34, 232));
        glass.setColorAt(0.50, QColor(17, 24, 31, 226));
        glass.setColorAt(1.0, QColor(14, 17, 23, 238));
        p.fillPath(card_path, glass);

        QLinearGradient border(
            QPointF(r.left() + std::sin(seconds) * r.width() * 0.5, r.top()),
            QPointF(r.right(), r.bottom() + std::cos(seconds) * r.height() * 0.3));
        border.setColorAt(0.0, QColor(34, 197, 94, 48));
        border.setColorAt(0.38, QColor(52, 211, 153, 185));
        border.setColorAt(0.72, QColor(148, 163, 184, 68));
        border.setColorAt(1.0, QColor(34, 197, 94, 54));
        QPen pen(QBrush(border), 1.9);
        p.setPen(pen);
        p.drawRoundedRect(r, radius, radius);
    }

private:
    QElapsedTimer m_clock;
};

class RippleButton final : public QPushButton
{
public:
    explicit RippleButton(const QString& text, QWidget* parent = nullptr)
        : QPushButton(text, parent)
    {
        setCursor(Qt::PointingHandCursor);
        setMinimumHeight(54);
        setIconSize(QSize(20, 20));
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        m_shadow = new QGraphicsDropShadowEffect(this);
        m_shadow->setBlurRadius(20);
        m_shadow->setOffset(0, 8);
        m_shadow->setColor(QColor(0, 0, 0, 88));
        setGraphicsEffect(m_shadow);
    }

protected:
    void enterEvent(QEnterEvent* event) override
    {
        animateShadow(30, QColor(34, 197, 94, 80));
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override
    {
        animateShadow(20, QColor(0, 0, 0, 88));
        QPushButton::leaveEvent(event);
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        m_ripple_center = event->position();
        startRipple();
        QPushButton::mousePressEvent(event);
    }

    void paintEvent(QPaintEvent* event) override
    {
        QPushButton::paintEvent(event);
        if (m_ripple <= 0.0 || m_ripple >= 1.0) return;

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const qreal max_radius = std::hypot(width(), height());
        QColor c(52, 211, 153);
        c.setAlphaF((1.0 - m_ripple) * 0.18);
        p.setBrush(c);
        p.setPen(Qt::NoPen);
        p.drawEllipse(m_ripple_center, max_radius * m_ripple, max_radius * m_ripple);
    }

private:
    void animateShadow(int blur, const QColor& color)
    {
        auto* anim = new QVariantAnimation(this);
        anim->setDuration(170);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setStartValue(m_shadow->blurRadius());
        anim->setEndValue(blur);
        connect(anim, &QVariantAnimation::valueChanged, this, [this, color](const QVariant& v) {
            m_shadow->setBlurRadius(v.toReal());
            m_shadow->setColor(color);
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    void startRipple()
    {
        auto* anim = new QVariantAnimation(this);
        anim->setDuration(420);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setStartValue(0.0);
        anim->setEndValue(1.0);
        connect(anim, &QVariantAnimation::valueChanged, this, [this](const QVariant& v) {
            m_ripple = v.toReal();
            update();
        });
        connect(anim, &QVariantAnimation::finished, this, [this] {
            m_ripple = 0.0;
            update();
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QGraphicsDropShadowEffect* m_shadow{nullptr};
    QPointF m_ripple_center;
    qreal m_ripple{0.0};
};

class HoverCard final : public QFrame
{
public:
    explicit HoverCard(QWidget* parent = nullptr)
        : QFrame(parent)
    {
        setAttribute(Qt::WA_Hover);
        m_shadow = new QGraphicsDropShadowEffect(this);
        m_shadow->setBlurRadius(18);
        m_shadow->setOffset(0, 8);
        m_shadow->setColor(QColor(0, 0, 0, 82));
        setGraphicsEffect(m_shadow);
    }

protected:
    void enterEvent(QEnterEvent* event) override
    {
        setProperty("hovered", true);
        style()->unpolish(this);
        style()->polish(this);
        animateShadow(28, 11, QColor(34, 197, 94, 62));
        QFrame::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override
    {
        setProperty("hovered", false);
        style()->unpolish(this);
        style()->polish(this);
        animateShadow(18, 8, QColor(0, 0, 0, 82));
        QFrame::leaveEvent(event);
    }

private:
    void animateShadow(int blur, int y, const QColor& color)
    {
        auto* anim = new QVariantAnimation(this);
        anim->setDuration(160);
        anim->setEasingCurve(QEasingCurve::OutCubic);
        anim->setStartValue(m_shadow->blurRadius());
        anim->setEndValue(blur);
        connect(anim, &QVariantAnimation::valueChanged, this, [this, y, color](const QVariant& v) {
            m_shadow->setBlurRadius(v.toReal());
            m_shadow->setOffset(0, y);
            m_shadow->setColor(color);
        });
        anim->start(QAbstractAnimation::DeleteWhenStopped);
    }

    QGraphicsDropShadowEffect* m_shadow{nullptr};
};

QString CompactText(const QString& text, const int max_chars)
{
    if (text.size() <= max_chars) return text;
    const int left = (max_chars - 3) / 2;
    const int right = max_chars - 3 - left;
    return text.left(left) + QStringLiteral("...") + text.right(right);
}

QString AmountWithoutUnit(const QString& formatted)
{
    const int unit_pos = formatted.lastIndexOf(QLatin1Char(' '));
    return unit_pos >= 0 ? formatted.left(unit_pos) : formatted;
}

QIcon ActionIcon(const QString& kind, const QColor& color = QColor(248, 250, 252))
{
    QPixmap pixmap(44, 44);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 2.7);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    if (kind == QLatin1String("send")) {
        p.drawLine(QPointF(14, 30), QPointF(30, 14));
        p.drawLine(QPointF(20, 14), QPointF(30, 14));
        p.drawLine(QPointF(30, 14), QPointF(30, 24));
    } else if (kind == QLatin1String("receive")) {
        p.drawLine(QPointF(22, 11), QPointF(22, 29));
        p.drawLine(QPointF(14, 21), QPointF(22, 29));
        p.drawLine(QPointF(30, 21), QPointF(22, 29));
        p.drawRoundedRect(QRectF(13, 31, 18, 3), 1.5, 1.5);
    } else if (kind == QLatin1String("transactions")) {
        p.drawRoundedRect(QRectF(11, 12, 22, 20), 5, 5);
        p.drawLine(QPointF(16, 18), QPointF(28, 18));
        p.drawLine(QPointF(16, 24), QPointF(25, 24));
    } else {
        p.drawRoundedRect(QRectF(12, 11, 18, 22), 4, 4);
        p.drawLine(QPointF(18, 11), QPointF(18, 33));
        p.drawEllipse(QPointF(25, 19), 2.8, 2.8);
        p.drawArc(QRectF(21, 22, 8, 8), 20 * 16, 140 * 16);
    }
    return QIcon(pixmap);
}

QPixmap ActivityGlyph(const QString& kind)
{
    const QColor color = kind == QLatin1String("send") ? QColor(239, 68, 68) :
                         kind == QLatin1String("mining") ? QColor(59, 130, 246) :
                         kind == QLatin1String("pending") ? QColor(148, 163, 184) :
                         QColor(34, 197, 94);
    QPixmap pixmap(30, 30);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(color, 2.2);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);

    if (kind == QLatin1String("send")) {
        p.drawLine(QPointF(10, 21), QPointF(20, 11));
        p.drawLine(QPointF(14, 11), QPointF(20, 11));
        p.drawLine(QPointF(20, 11), QPointF(20, 17));
    } else if (kind == QLatin1String("mining")) {
        p.drawRoundedRect(QRectF(9, 12, 12, 10), 2.5, 2.5);
        p.drawLine(QPointF(11, 10), QPointF(19, 10));
        p.drawLine(QPointF(15, 7), QPointF(15, 10));
    } else if (kind == QLatin1String("pending")) {
        p.drawEllipse(QPointF(15, 15), 8, 8);
        p.drawLine(QPointF(15, 15), QPointF(15, 10));
        p.drawLine(QPointF(15, 15), QPointF(19, 17));
    } else {
        p.drawLine(QPointF(15, 8), QPointF(15, 20));
        p.drawLine(QPointF(10, 15), QPointF(15, 20));
        p.drawLine(QPointF(20, 15), QPointF(15, 20));
    }
    return pixmap;
}

QString HeroBalanceHtml(const QString& formatted)
{
    const int unit_pos = formatted.lastIndexOf(QLatin1Char(' '));
    const QString amount = unit_pos >= 0 ? formatted.left(unit_pos) : formatted;
    const QString unit = unit_pos >= 0 ? formatted.mid(unit_pos + 1) : QStringLiteral("KPEPE");
    const int decimal_pos = amount.indexOf(QLatin1Char('.'));
    const QString whole = decimal_pos >= 0 ? amount.left(decimal_pos) : amount;
    const QString decimals = decimal_pos >= 0 ? amount.mid(decimal_pos) : QString();
    return QStringLiteral("<span style=\"font-size:43px; font-weight:900; color:#22C55E;\">%1</span>"
                          "<span style=\"font-size:21px; font-weight:750; color:#34D399;\">%2</span>"
                          "<br><span style=\"font-size:18px; font-weight:850; color:#F8FAFC;\">%3</span>")
        .arg(whole.toHtmlEscaped(), decimals.toHtmlEscaped(), unit.toHtmlEscaped());
}

QString RelativeTime(const QDateTime& when)
{
    if (!when.isValid()) return QStringLiteral("now");
    const qint64 secs = when.secsTo(QDateTime::currentDateTime());
    if (secs < 45) return QObject::tr("now");
    if (secs < 90) return QObject::tr("1m ago");
    if (secs < 3600) return QObject::tr("%1m ago").arg(secs / 60);
    if (secs < 7200) return QObject::tr("1h ago");
    if (secs < 86400) return QObject::tr("%1h ago").arg(secs / 3600);
    if (secs < 172800) return QObject::tr("1d ago");
    return QObject::tr("%1d ago").arg(secs / 86400);
}

void FadeIn(QWidget* widget, int duration = 260)
{
    auto* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    auto* anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(duration);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}
} // namespace

DashboardPage::DashboardPage(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("dashboardPage"));

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QScrollArea* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* container = new QWidget(scroll);
    QVBoxLayout* col = new QVBoxLayout(container);
    col->setContentsMargins(16, 16, 16, 18);
    col->setSpacing(18);

    // --- Hero balance card ---
    AnimatedHeroCard* balanceCard = new AnimatedHeroCard(container);
    QVBoxLayout* hero = new QVBoxLayout(balanceCard);
    hero->setContentsMargins(20, 19, 20, 19);
    hero->setSpacing(9);

    QHBoxLayout* heroTop = new QHBoxLayout();
    heroTop->setSpacing(8);
    QLabel* heroLogo = new QLabel(balanceCard);
    QPixmap heroPixmap(QStringLiteral(":/icons/bitcoin"));
    if (!heroPixmap.isNull()) {
        heroLogo->setPixmap(heroPixmap.scaled(23, 23, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    QLabel* heroTitle = new QLabel(tr("Total balance"), balanceCard);
    heroTitle->setObjectName(QStringLiteral("heroTitle"));
    heroTop->addWidget(heroLogo);
    heroTop->addWidget(heroTitle);
    heroTop->addStretch(1);
    hero->addLayout(heroTop);

    m_total = new QLabel(QStringLiteral("-"), balanceCard);
    m_total->setObjectName(QStringLiteral("dashTotal"));
    m_total->setTextFormat(Qt::RichText);
    m_total->setWordWrap(true);
    m_total->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_total->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    hero->addWidget(m_total);
    QVBoxLayout* subRow = new QVBoxLayout();
    subRow->setSpacing(4);
    m_available = new QLabel(balanceCard); m_available->setObjectName(QStringLiteral("dashSub"));
    m_pending = new QLabel(balanceCard);   m_pending->setObjectName(QStringLiteral("dashSub"));
    m_immature = new QLabel(balanceCard);  m_immature->setObjectName(QStringLiteral("dashSub"));
    subRow->addWidget(m_available);
    subRow->addWidget(m_pending);
    subRow->addWidget(m_immature);
    subRow->addStretch(1);
    hero->addLayout(subRow);
    m_sync = new QLabel(balanceCard); m_sync->setObjectName(QStringLiteral("dashSyncNote"));
    hero->addWidget(m_sync);
    col->addWidget(balanceCard);

    // --- Quick actions ---
    QGridLayout* actions = new QGridLayout();
    actions->setHorizontalSpacing(12);
    actions->setVerticalSpacing(12);
    RippleButton* bSend = new RippleButton(tr("Send"), container); bSend->setObjectName(QStringLiteral("qaBtn")); bSend->setProperty("primary", true);
    RippleButton* bRecv = new RippleButton(tr("Receive"), container); bRecv->setObjectName(QStringLiteral("qaBtn"));
    RippleButton* bTx = new RippleButton(tr("Transactions"), container); bTx->setObjectName(QStringLiteral("qaBtn"));
    RippleButton* bAddr = new RippleButton(tr("Address Book"), container); bAddr->setObjectName(QStringLiteral("qaBtn"));
    bSend->setIcon(ActionIcon(QStringLiteral("send"), QColor(14, 17, 23)));
    bRecv->setIcon(ActionIcon(QStringLiteral("receive")));
    bTx->setIcon(ActionIcon(QStringLiteral("transactions")));
    bAddr->setIcon(ActionIcon(QStringLiteral("addressbook")));
    connect(bSend, &QPushButton::clicked, this, &DashboardPage::sendRequested);
    connect(bRecv, &QPushButton::clicked, this, &DashboardPage::receiveRequested);
    connect(bTx, &QPushButton::clicked, this, &DashboardPage::transactionsRequested);
    connect(bAddr, &QPushButton::clicked, this, &DashboardPage::addressBookRequested);
    actions->addWidget(bSend, 0, 0);
    actions->addWidget(bRecv, 0, 1);
    actions->addWidget(bTx, 1, 0);
    actions->addWidget(bAddr, 1, 1);
    actions->setColumnStretch(0, 1);
    actions->setColumnStretch(1, 1);
    col->addLayout(actions);

    // --- Assets card ---
    WalletCard* assetsCard = new WalletCard(tr("Assets"), container);
    HoverCard* assetCard = new HoverCard(container);
    assetCard->setObjectName(QStringLiteral("assetRowCard"));
    QHBoxLayout* assetRow = new QHBoxLayout(assetCard);
    assetRow->setContentsMargins(14, 12, 14, 12);
    assetRow->setSpacing(12);
    QLabel* assetIcon = new QLabel(assetCard);
    assetIcon->setObjectName(QStringLiteral("assetIcon"));
    QPixmap assetPixmap(QStringLiteral(":/icons/bitcoin"));
    if (!assetPixmap.isNull()) {
        assetIcon->setPixmap(assetPixmap.scaled(25, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    QVBoxLayout* assetText = new QVBoxLayout();
    assetText->setContentsMargins(0, 0, 0, 0);
    assetText->setSpacing(1);
    QLabel* assetName = new QLabel(tr("KPEPE"), assetCard);
    assetName->setObjectName(QStringLiteral("assetName"));
    QLabel* assetMeta = new QLabel(tr("KingPepe native"), assetCard);
    assetMeta->setObjectName(QStringLiteral("assetMeta"));
    assetText->addWidget(assetName);
    assetText->addWidget(assetMeta);
    QVBoxLayout* assetAmounts = new QVBoxLayout();
    assetAmounts->setContentsMargins(0, 0, 0, 0);
    assetAmounts->setSpacing(1);
    m_assetBalance = new QLabel(QStringLiteral("-"), assetCard);
    m_assetBalance->setObjectName(QStringLiteral("assetBalance"));
    m_assetBalance->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_assetBalance->setWordWrap(false);
    assetAmounts->addWidget(m_assetBalance);
    assetRow->addWidget(assetIcon);
    assetRow->addLayout(assetText, 1);
    assetRow->addStretch(1);
    assetRow->addLayout(assetAmounts);
    assetsCard->addBodyWidget(assetCard);
    col->addWidget(assetsCard);

    // --- Recent activity card ---
    WalletCard* activityCard = new WalletCard(tr("Recent activity"), container);
    QWidget* actWrap = new QWidget(container);
    m_activityList = new QVBoxLayout(actWrap);
    m_activityList->setContentsMargins(0, 0, 0, 0);
    m_activityList->setSpacing(6);
    m_activityList->addStretch(1);
    activityCard->addBodyWidget(actWrap);
    col->addWidget(activityCard);

    // --- Network card ---
    WalletCard* netCard = new WalletCard(tr("Network"), container);
    auto addKV = [&](const QString& key, QLabel** valPtr) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* k = new QLabel(key, container); k->setObjectName(QStringLiteral("kvKey"));
        QLabel* v = new QLabel(QStringLiteral("-"), container); v->setObjectName(QStringLiteral("kvVal"));
        v->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        row->addWidget(k);
        row->addStretch(1);
        row->addWidget(v);
        netCard->addBodyLayout(row);
        *valPtr = v;
    };
    addKV(tr("Network status"), &m_network);
    addKV(tr("Block height"), &m_height);
    addKV(tr("Connections"), &m_conn);
    addKV(tr("Sync status"), &m_syncStatus);
    addKV(tr("Sync progress"), &m_syncPct);
    col->addWidget(netCard);

    col->addStretch(1);
    scroll->setWidget(container);
    root->addWidget(scroll);
}

void DashboardPage::setBalance(const QString& total, const QString& available,
                               const QString& pending,
                               const QString& immature)
{
    m_total->setText(HeroBalanceHtml(total));
    m_assetBalance->setText(AmountWithoutUnit(total));
    m_available->setText(tr("Available %1").arg(available));
    m_pending->setText(tr("Pending %1").arg(pending));
    m_immature->setText(tr("Immature %1").arg(immature));
}

void DashboardPage::setBlockHeight(const QString& height) { m_height->setText(height); }
void DashboardPage::setConnections(const QString& peers) { m_conn->setText(peers); }
void DashboardPage::setNetworkStatus(const QString& status) { m_network->setText(status); }
void DashboardPage::setSyncProgress(const QString& progress, const QString& status)
{
    m_syncPct->setText(progress);
    m_syncStatus->setText(status);
    m_sync->setText(tr("%1 - %2").arg(status, progress));
}

void DashboardPage::setWalletModel(WalletModel* model)
{
    m_walletModel = model;
    if (!model) return;
    TransactionTableModel* ttm = model->getTransactionTableModel();
    if (!ttm) return;
    connect(ttm, &QAbstractItemModel::modelReset, this, &DashboardPage::rebuildActivity);
    connect(ttm, &QAbstractItemModel::rowsInserted, this, &DashboardPage::rebuildActivity);
    connect(ttm, &QAbstractItemModel::rowsRemoved, this, &DashboardPage::rebuildActivity);
    connect(ttm, &QAbstractItemModel::dataChanged, this, &DashboardPage::rebuildActivity);
    connect(ttm, &QAbstractItemModel::layoutChanged, this, &DashboardPage::rebuildActivity);
    rebuildActivity();
}

void DashboardPage::rebuildActivity()
{
    if (!m_walletModel || !m_activityList) return;
    TransactionTableModel* ttm = m_walletModel->getTransactionTableModel();
    if (!ttm) return;

    while (m_activityList->count() > 1) {
        QLayoutItem* item = m_activityList->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    const int rows = ttm->rowCount(QModelIndex());
    std::vector<int> row_order(rows);
    std::iota(row_order.begin(), row_order.end(), 0);
    std::sort(row_order.begin(), row_order.end(), [ttm](int a, int b) {
        const QDateTime ad = ttm->index(a, 0).data(TransactionTableModel::DateRole).toDateTime();
        const QDateTime bd = ttm->index(b, 0).data(TransactionTableModel::DateRole).toDateTime();
        if (ad == bd) return a > b;
        return ad > bd;
    });

    int added = 0;
    for (const int r : row_order) {
        if (added >= 6) break;
        const QModelIndex idx = ttm->index(r, 0);
        const bool confirmed = idx.data(TransactionTableModel::ConfirmedRole).toBool();
        const QString type = ttm->index(r, TransactionTableModel::Type).data().toString();
        const QDateTime when = idx.data(TransactionTableModel::DateRole).toDateTime();
        const QString label = idx.data(TransactionTableModel::LabelRole).toString();
        const QString address = idx.data(TransactionTableModel::AddressRole).toString();
        const QString amount = idx.data(TransactionTableModel::FormattedAmountRole).toString();
        const QString identity = !label.isEmpty() ? label : address;
        const QString primary = !type.isEmpty() ? type : tr("Transaction");
        const QString secondary = (!identity.isEmpty() ? CompactText(identity, 22) : tr("Wallet")) +
                                  QStringLiteral("  -  ") + RelativeTime(when);
        QString badge_kind = QStringLiteral("pending");
        if (primary.contains(tr("Mined"), Qt::CaseInsensitive)) {
            badge_kind = QStringLiteral("mining");
        } else if (confirmed) {
            badge_kind = amount.startsWith(QLatin1Char('-')) ? QStringLiteral("send") : QStringLiteral("receive");
        }

        QFrame* row = new QFrame();
        row->setObjectName(QStringLiteral("actRow"));
        QHBoxLayout* h = new QHBoxLayout(row);
        h->setContentsMargins(10, 9, 10, 9);
        h->setSpacing(10);
        QLabel* icon = new QLabel(row);
        icon->setObjectName(QStringLiteral("actIcon"));
        icon->setProperty("kind", badge_kind);
        icon->setPixmap(ActivityGlyph(badge_kind));
        icon->setAlignment(Qt::AlignCenter);
        QVBoxLayout* textCol = new QVBoxLayout();
        textCol->setContentsMargins(0, 0, 0, 0);
        textCol->setSpacing(1);
        QLabel* who = new QLabel(CompactText(primary, 24), row);
        who->setObjectName(QStringLiteral("actWho"));
        who->setToolTip(primary);
        QLabel* meta = new QLabel(CompactText(secondary, 30), row);
        meta->setObjectName(QStringLiteral("actMeta"));
        meta->setToolTip(secondary);
        QLabel* badge = new QLabel(badge_kind == QLatin1String("send") ? tr("Send") :
                                   badge_kind == QLatin1String("mining") ? tr("Mining") :
                                   badge_kind == QLatin1String("pending") ? tr("Pending") :
                                   tr("Receive"), row);
        badge->setObjectName(QStringLiteral("actBadge"));
        badge->setProperty("kind", badge_kind);
        QLabel* amt = new QLabel(amount, row);
        amt->setObjectName(amount.startsWith('-') ? QStringLiteral("actAmtOut") : QStringLiteral("actAmtIn"));
        amt->setProperty("kind", badge_kind);
        amt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        textCol->addWidget(who);
        textCol->addWidget(meta);
        QVBoxLayout* rightCol = new QVBoxLayout();
        rightCol->setContentsMargins(0, 0, 0, 0);
        rightCol->setSpacing(4);
        rightCol->addWidget(amt, 0, Qt::AlignRight);
        rightCol->addWidget(badge, 0, Qt::AlignRight);
        h->addWidget(icon);
        h->addLayout(textCol, 1);
        h->addLayout(rightCol);
        m_activityList->insertWidget(m_activityList->count() - 1, row);
        FadeIn(row, 220);
        ++added;
    }

    if (added == 0) {
        QLabel* empty = new QLabel(tr("No transactions yet."));
        empty->setObjectName(QStringLiteral("actEmpty"));
        m_activityList->insertWidget(m_activityList->count() - 1, empty);
    }
}
