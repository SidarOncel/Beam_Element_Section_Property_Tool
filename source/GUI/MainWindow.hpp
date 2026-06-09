#pragma once

/*
    MainWindow.hpp

    This file defines the main application window for the Beam Element Section
    Property Tool.

    What this window is responsible for:
    - Hosting the three main tabs: General, Stress Points, FE Mesh
    - Keeping the current section input in sync across the tabs
    - Providing user actions such as Save Project, Load Project, and Export Results
    - Connecting the GUI to the backend core modules

    What this window is NOT responsible for:
    - Doing geometry calculations itself
    - Doing property calculations itself
    - Writing export files itself
    - Generating meshes itself

    Those tasks belong to the core modules. The window only triggers them.
*/

#include "../Geometry/SectionInput.hpp"
#include "../Core/PropertyCalculator.hpp"
#include "MeshGenerator.hpp"

#include <QMainWindow>

// Forward declarations keep the header lighter and reduce compile time.
// The full class definitions are not needed here.
class FEMeshTab;
class GeneralTab;
class QTabWidget;
class StressPointsTab;
class QWidget;
class MeshDisplayWidget;
class QAction;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    // Triggered when the user asks the FE Mesh tab to generate a mesh.
    void onGenerateMeshClicked();

    // Triggered from the File menu.
    void onSaveProjectTriggered();

    // Triggered from the File menu.
    void onLoadProjectTriggered();

    // Triggered from the File menu.
    void onExportResultsTriggered();

private:
    // Creates the General tab and keeps the pointer for later updates.
    GeneralTab* createGeneralTab();

    // Creates the Stress Points tab and keeps the pointer for later updates.
    StressPointsTab* createStressPointsTab();

    // Creates the FE Mesh tab and keeps the pointer for later updates.
    FEMeshTab* createFEMeshTab();

    // Called whenever the General tab changes the section input.
    void handleSectionInputChanged(const SectionInput& input);

    // Pushes the current input to the tabs that depend on it.
    void refreshDependentTabs();

    // Returns the current section properties by calling the correct backend formula.
    SectionProperties calculateCurrentProperties() const;

private:
    // The current section input shown in the GUI.
    SectionInput m_currentInput;

    // Main tab container.
    QTabWidget* m_tabWidget = nullptr;

    // Individual tabs.
    FEMeshTab* m_feMeshTab = nullptr;
    GeneralTab* m_generalTab = nullptr;
    StressPointsTab* m_stressPointsTab = nullptr;

    // Widget used to show the FE mesh visualization.
    MeshDisplayWidget* m_meshDisplayWidget = nullptr;

    // File menu actions.
    QAction* m_saveProjectAction = nullptr;
    QAction* m_loadProjectAction = nullptr;
    QAction* m_exportResultsAction = nullptr;
};