#include "MeshDisplayWidget.hpp"
#include <limits>
#include <QPen>
#include <QBrush>

MeshDisplayWidget::MeshDisplayWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(400, 300);
    setAutoFillBackground(true);
}

void MeshDisplayWidget::setMeshData(const MeshData& data)
{
    m_meshData = data;
    update();
}

void MeshDisplayWidget::clear()
{
    m_meshData = MeshData();
    update();
}

QPointF MeshDisplayWidget::toQPointF(const Point2D& p) const
{
    return QPointF(p.y, p.z);
}

void MeshDisplayWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(250, 251, 252));
    
    if (!m_meshData.isValid || m_meshData.triangles.empty())
    {
        painter.setPen(QColor(115, 120, 128));
        painter.drawText(rect(), Qt::AlignCenter, 
                        "No mesh generated.\nSet mesh size and click 'Generate Mesh'.");
        return;
    }
    
    // Find bounds
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    double minZ = std::numeric_limits<double>::max();
    double maxZ = std::numeric_limits<double>::lowest();
    
    for (const auto& tri : m_meshData.triangles)
    {
        for (const auto& p : {tri.p1, tri.p2, tri.p3})
        {
            minY = std::min(minY, p.y);
            maxY = std::max(maxY, p.y);
            minZ = std::min(minZ, p.z);
            maxZ = std::max(maxZ, p.z);
        }
    }
    
    double widthVal = maxY - minY;
    double heightVal = maxZ - minZ;
    double margin = 40.0;
    double w = this->width();
    double h = this->height();
    
    double scaleX = (w - 2 * margin) / widthVal;
    double scaleY = (h - 2 * margin) / heightVal;
    double scale = std::min(scaleX, scaleY);
    
    double offsetX = margin + (w - 2 * margin - widthVal * scale) / 2 - minY * scale;
    double offsetY = margin + (h - 2 * margin - heightVal * scale) / 2 - minZ * scale;
    
    painter.setPen(QPen(QColor(80, 90, 100), 1.0));
    painter.setBrush(QBrush(QColor(190, 211, 230, 120)));
    
    for (const auto& tri : m_meshData.triangles)
    {
        QPolygonF polygon;
        
        double x1 = tri.p1.y * scale + offsetX;
        double y1 = h - (tri.p1.z * scale + offsetY);
        double x2 = tri.p2.y * scale + offsetX;
        double y2 = h - (tri.p2.z * scale + offsetY);
        double x3 = tri.p3.y * scale + offsetX;
        double y3 = h - (tri.p3.z * scale + offsetY);
        
        polygon << QPointF(x1, y1) << QPointF(x2, y2) << QPointF(x3, y3);
        painter.drawPolygon(polygon);
    }
}

void MeshDisplayWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}