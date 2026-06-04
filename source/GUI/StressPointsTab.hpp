#pragma once

#include "StressPointDefaults.hpp"

#include <QString>
#include <QWidget>

#include <vector>

class QTableWidget;
class SectionPreviewWidget;
class QLabel;

class StressPointsTab : public QWidget
{
public:
    explicit StressPointsTab(QWidget* parent = nullptr);

    void setSectionInput(const SectionInput& input);
    const SectionInput& sectionInput() const;
    std::vector<StressPoint> stressPoints() const;

private:
    void setupUi();
    void resetToDefaultStressPoints();
    void updateTableFromPoints();
    void updatePreview();
    void setStatusMessage(const QString& message, bool isError);
    void handleCellChanged(int row, int column);

    SectionInput m_input;
    std::vector<StressPoint> m_points;
    QLabel* m_statusLabel = nullptr;
    QTableWidget* m_table = nullptr;
    SectionPreviewWidget* m_previewWidget = nullptr;
    bool m_isUpdating = false;
};
