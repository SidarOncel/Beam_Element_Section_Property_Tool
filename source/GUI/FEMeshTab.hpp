#pragma once

#include "../geometry/SectionInput.hpp"

#include <QWidget>

class QLabel;
class QDoubleSpinBox;
class QPushButton;
class QVBoxLayout;

class FEMeshTab : public QWidget
{
public:
    explicit FEMeshTab(QWidget* parent = nullptr);

    void setSectionInput(const SectionInput& input);
    const SectionInput& sectionInput() const;

    double requestedMeshSize() const;
    void setMeshControlsEnabled(bool enabled);
    QPushButton* generateButton() const;
    QWidget* visualizationHost() const;
    void setExternalMeshWidget(QWidget* widget);
    void clearExternalMeshWidget();

private:
    void setupUi();
    void updateSectionSummary();
    void updateStatus();

    SectionInput m_input;
    QLabel* m_sectionSummaryLabel = nullptr;
    QLabel* m_statusLabel = nullptr;
    QLabel* m_placeholderLabel = nullptr;
    QDoubleSpinBox* m_meshSizeSpinBox = nullptr;
    QPushButton* m_generateButton = nullptr;
    QWidget* m_visualizationHost = nullptr;
    QVBoxLayout* m_visualizationHostLayout = nullptr;
    QWidget* m_externalMeshWidget = nullptr;
};
