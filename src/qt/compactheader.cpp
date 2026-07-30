// Copyright (c) 2024-present The KingPepe developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/compactheader.h>

#include <cmath>

#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QElapsedTimer>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QSize>
#include <QSizePolicy>
#include <QTimer>
#include <QToolButton>
#include <QVariantAnimation>

namespace {
QIcon SettingsIcon()
{
    QPixmap pixmap(36, 36);
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    QPen pen(QColor(248, 250, 252), 2.1);
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);

    auto drawSlider = [&p](qreal y, qreal knob_x) {
        p.drawLine(QPointF(9, y), QPointF(27, y));
        p.setBrush(QColor(22, 27, 34));
        p.drawEllipse(QPointF(knob_x, y), 3.7, 3.7);
    };
    drawSlider(11, 16);
    drawSlider(18, 23);
    drawSlider(25, 13);
    return QIcon(pixmap);
}

class IndicatorDot final : public QWidget
{
public:
    explicit IndicatorDot(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setFixedSize(12, 12);
        m_clock.start();
        auto* timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [this] { update(); });
        timer->start(80);
    }

    void setColor(const QColor& color)
    {
        m_color = color;
        update();
    }

    void setActive(bool active)
    {
        m_active = active;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QPointF c(width() / 2.0, height() / 2.0);
        const qreal pulse = m_active ? ((std::sin(m_clock.elapsed() / 520.0) + 1.0) * 0.5) : 0.0;
        QColor ring = m_color;
        ring.setAlpha(35 + static_cast<int>(pulse * 70));
        p.setPen(Qt::NoPen);
        p.setBrush(ring);
        p.drawEllipse(c, 5.2 + pulse * 2.0, 5.2 + pulse * 2.0);
        p.setBrush(m_color);
        p.drawEllipse(c, 3.5, 3.5);
    }

private:
    QElapsedTimer m_clock;
    QColor m_color{34, 197, 94};
    bool m_active{false};
};

class HeaderIconButton final : public QToolButton
{
public:
    explicit HeaderIconButton(QWidget* parent = nullptr)
        : QToolButton(parent)
    {
        setCursor(Qt::PointingHandCursor);
        m_shadow = new QGraphicsDropShadowEffect(this);
        m_shadow->setBlurRadius(0);
        m_shadow->setOffset(0, 0);
        m_shadow->setColor(QColor(34, 197, 94, 0));
        setGraphicsEffect(m_shadow);
    }

protected:
    void enterEvent(QEnterEvent* event) override
    {
        animateShadow(18, QColor(34, 197, 94, 76));
        QToolButton::enterEvent(event);
    }

    void leaveEvent(QEvent* event) override
    {
        animateShadow(0, QColor(34, 197, 94, 0));
        QToolButton::leaveEvent(event);
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

    QGraphicsDropShadowEffect* m_shadow{nullptr};
};
} // namespace

CompactHeader::CompactHeader(QWidget* parent)
    : QWidget(parent)
{
    setObjectName(QStringLiteral("compactHeader"));
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(14, 8, 12, 8);
    layout->setSpacing(8);

    // KingPepe logo (own brand asset from resources).
    QLabel* logo = new QLabel(this);
    logo->setObjectName(QStringLiteral("compactLogo"));
    QPixmap px(QStringLiteral(":/icons/bitcoin"));
    if (!px.isNull()) {
        logo->setPixmap(px.scaled(26, 26, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    m_name = new QLabel(tr("KingPepe"), this);
    m_name->setObjectName(QStringLiteral("compactWalletName"));

    m_network = new QLabel(this);
    m_network->setObjectName(QStringLiteral("compactNetwork"));

    m_lock = new QLabel(this);
    m_lock->setObjectName(QStringLiteral("compactLock"));

    m_sync_indicator = new IndicatorDot(this);
    m_sync_indicator->setObjectName(QStringLiteral("compactSyncDot"));
    static_cast<IndicatorDot*>(m_sync_indicator)->setColor(QColor(148, 163, 184));

    m_connection_indicator = new IndicatorDot(this);
    m_connection_indicator->setObjectName(QStringLiteral("compactConnDot"));
    static_cast<IndicatorDot*>(m_connection_indicator)->setColor(QColor(148, 163, 184));
    m_connections = new QLabel(QStringLiteral("0"), this);
    m_connections->setObjectName(QStringLiteral("compactConnections"));

    QToolButton* settings = new HeaderIconButton(this);
    settings->setObjectName(QStringLiteral("compactSettingsButton"));
    settings->setIcon(SettingsIcon());
    settings->setIconSize(QSize(18, 18));
    settings->setToolButtonStyle(Qt::ToolButtonIconOnly);
    settings->setAutoRaise(true);
    settings->setToolTip(tr("Settings"));
    connect(settings, &QToolButton::clicked, this, &CompactHeader::settingsRequested);

    layout->addWidget(logo);
    layout->addWidget(m_name);
    layout->addStretch(1);
    layout->addWidget(m_network);
    layout->addWidget(m_lock);
    layout->addWidget(m_sync_indicator);
    layout->addWidget(m_connection_indicator);
    layout->addWidget(m_connections);
    layout->addWidget(settings);
}

void CompactHeader::setWalletName(const QString& name)
{
    m_name->setText(tr("KingPepe"));
    m_name->setToolTip(name.isEmpty() ? tr("KingPepe") : name);
}

void CompactHeader::setNetwork(const QString& network)
{
    m_network->setText(network);
}

void CompactHeader::setLockStatus(const QString& status)
{
    m_lock->setText(status);
}

void CompactHeader::setConnections(int count)
{
    m_connections->setText(QString::number(count));
    static_cast<IndicatorDot*>(m_connection_indicator)->setColor(count > 0 ? QColor(34, 197, 94) : QColor(148, 163, 184));
    m_connection_indicator->setToolTip(tr("%n active connection(s)", "", count));
}

void CompactHeader::setSyncActive(bool active)
{
    auto* dot = static_cast<IndicatorDot*>(m_sync_indicator);
    dot->setActive(active);
    dot->setColor(active ? QColor(34, 197, 94) : QColor(148, 163, 184));
    dot->setToolTip(active ? tr("Syncing") : tr("Synced"));
}
