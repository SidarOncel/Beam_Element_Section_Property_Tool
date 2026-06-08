#pragma once

#include "../geometry/SectionInput.hpp"
#include "MeshGenerator.hpp"
#include <QMainWindow>

class FEMeshTab;
class GeneralTab;
class QTabWidget;
class StressPointsTab;
class QWidget;
class MeshDisplayWidget;
class MainWindow : public QMainWindow
{
     Q_OBJECT; 
public:
    explicit MainWindow(QWidget* parent = nullptr);
private slots:  
    void onGenerateMeshClicked();
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
    MeshDisplayWidget* m_meshDisplayWidget = nullptr;
};
