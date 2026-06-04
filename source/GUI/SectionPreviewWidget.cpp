#include "SectionPreviewWidget.hpp"

#include <QFont>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

#include <algorithm>
#include <limits>

namespace
{
    struct Bounds
    {
        double minY = 0.0;
        double maxY = 0.0;
        double minZ = 0.0;
        double maxZ = 0.0;
    };

    void includePoint(Bounds& bounds, const Point2D& point)
    {
        bounds.minY = std::min(bounds.minY, point.y);
        bounds.maxY = std::max(bounds.maxY, point.y);
        bounds.minZ = std::min(bounds.minZ, point.z);
        bounds.maxZ = std::max(bounds.maxZ, point.z);
    }

    Bounds shapeBounds(const SectionShape& shape)
    {
        Bounds bounds;
        bounds.minY = std::numeric_limits<double>::max();
        bounds.maxY = std::numeric_limits<double>::lowest();
        bounds.minZ = std::numeric_limits<double>::max();
        bounds.maxZ = std::numeric_limits<double>::lowest();

        for (const Point2D& point : shape.boundaryPoints)
        {
            includePoint(bounds, point);
        }

        for (const auto& hole : shape.holes)
        {
            for (const Point2D& point : hole)
            {
                includePoint(bounds, point);
            }
        }

        if (bounds.minY > bounds.maxY || bounds.minZ > bounds.maxZ)
        {
            return {};
        }

        return bounds;
    }

    QPointF mapPoint(const Point2D& point,
                     const Bounds& bounds,
                     const QRectF& target)
    {
        const double width = std::max(bounds.maxY - bounds.minY, 1.0);
        const double height = std::max(bounds.maxZ - bounds.minZ, 1.0);
        const double scale = std::min(target.width() / width, target.height() / height);

        const double drawnWidth = width * scale;
        const double drawnHeight = height * scale;
        const double xOffset = target.left() + (target.width() - drawnWidth) / 2.0;
        const double yOffset = target.top() + (target.height() - drawnHeight) / 2.0;

        const double x = xOffset + (point.y - bounds.minY) * scale;
        const double y = yOffset + drawnHeight - (point.z - bounds.minZ) * scale;
        return {x, y};
    }

    void addPolygon(QPainterPath& path,
                    const std::vector<Point2D>& points,
                    const Bounds& bounds,
                    const QRectF& target)
    {
        if (points.empty())
        {
            return;
        }

        path.moveTo(mapPoint(points.front(), bounds, target));
        for (std::size_t i = 1; i < points.size(); ++i)
        {
            path.lineTo(mapPoint(points[i], bounds, target));
        }
        path.closeSubpath();
    }
}

SectionPreviewWidget::SectionPreviewWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(360, 320);
    setAutoFillBackground(true);
}

void SectionPreviewWidget::setShape(const SectionShape& shape)
{
    m_shape = shape;
    m_hasShape = !shape.boundaryPoints.empty();
    update();
}

void SectionPreviewWidget::setPointMarkers(const std::vector<PreviewPointMarker>& markers)
{
    m_pointMarkers = markers;
    update();
}

void SectionPreviewWidget::clearPointMarkers()
{
    m_pointMarkers.clear();
    update();
}

void SectionPreviewWidget::clearShape()
{
    m_shape = {};
    m_hasShape = false;
    m_pointMarkers.clear();
    update();
}

void SectionPreviewWidget::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), QColor(250, 251, 252));

    if (!m_hasShape)
    {
        painter.setPen(QColor(115, 120, 128));
        painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("Preview unavailable"));
        return;
    }

    const Bounds bounds = shapeBounds(m_shape);
    const QRectF target = rect().adjusted(24.0, 24.0, -24.0, -24.0);

    QPainterPath path;
    path.setFillRule(Qt::OddEvenFill);
    addPolygon(path, m_shape.boundaryPoints, bounds, target);
    for (const auto& hole : m_shape.holes)
    {
        addPolygon(path, hole, bounds, target);
    }

    painter.setPen(QPen(QColor(41, 52, 65), 2.0));
    painter.setBrush(QColor(190, 211, 230));
    painter.drawPath(path);

    painter.setPen(QPen(QColor(103, 113, 126), 1.0, Qt::DashLine));
    painter.drawLine(QPointF(target.left(), mapPoint({0.0, 0.0}, bounds, target).y()),
                     QPointF(target.right(), mapPoint({0.0, 0.0}, bounds, target).y()));
    painter.drawLine(QPointF(mapPoint({0.0, 0.0}, bounds, target).x(), target.top()),
                     QPointF(mapPoint({0.0, 0.0}, bounds, target).x(), target.bottom()));

    if (!m_pointMarkers.empty())
    {
        painter.setPen(QPen(QColor(146, 51, 43), 2.0));
        painter.setBrush(QColor(238, 91, 80));
        const QFont originalFont = painter.font();
        QFont markerFont = originalFont;
        markerFont.setBold(true);
        painter.setFont(markerFont);

        for (const PreviewPointMarker& marker : m_pointMarkers)
        {
            const QPointF center = mapPoint(marker.coordinates, bounds, target);
            painter.drawEllipse(center, 5.0, 5.0);
            painter.drawText(QRectF(center.x() + 7.0, center.y() - 14.0, 28.0, 18.0),
                             Qt::AlignLeft | Qt::AlignVCenter,
                             marker.label);
        }

        painter.setFont(originalFont);
    }
}
