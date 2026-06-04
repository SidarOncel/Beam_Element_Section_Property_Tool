#pragma once

#include "../geometry/SectionInput.hpp"

#include <QMainWindow>

class FEMeshTab;
class GeneralTab;
class QTabWidget;
class StressPointsTab;
class QWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    GeneralTab* createGeneralTab();
    StressPointsTab* createStressPointsTab();
    FEMeshTab* createFEMeshTab();
    void handleSectionInputChanged(const SectionInput& input);
    void refreshDependentTabs();

    SectionInput m_currentInput;
    QTabWidget* m_tabWidget = nullptr;
    FEMeshTab* m_feMeshTab = nullptr;
    GeneralTab* m_generalTab = nullptr;
    StressPointsTab* m_stressPointsTab = nullptr;
};
