#pragma once

#include "MeshGenerator.hpp"
#include <QWidget>
#include <QPainter>

class MeshDisplayWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MeshDisplayWidget(QWidget* parent = nullptr);
    
    void setMeshData(const MeshData& data);
    void clear();
    bool hasMesh() const { return m_meshData.isValid; }
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
private:
    QPointF toQPointF(const Point2D& p) const;
    
    MeshData m_meshData;
};