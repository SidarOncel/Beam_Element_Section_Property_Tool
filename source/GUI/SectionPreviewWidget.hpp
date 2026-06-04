#pragma once

#include "../geometry/SectionShape.hpp"

#include <QWidget>

#include <QString>

#include <vector>

struct PreviewPointMarker
{
    QString label;
    Point2D coordinates;
};

class SectionPreviewWidget : public QWidget
{
public:
    explicit SectionPreviewWidget(QWidget* parent = nullptr);

    void setShape(const SectionShape& shape);
    void setPointMarkers(const std::vector<PreviewPointMarker>& markers);
    void clearPointMarkers();
    void clearShape();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_hasShape = false;
    SectionShape m_shape;
    std::vector<PreviewPointMarker> m_pointMarkers;
};
