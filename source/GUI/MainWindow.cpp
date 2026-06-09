#include "MainWindow.hpp"

/*
    MainWindow.cpp

    This file implements the main application window.

    Responsibilities:
    - Create the three main tabs
    - Set up the File menu
    - Connect GUI actions to backend functions
    - Keep section data synchronized across tabs
    - Forward the current section to mesh and export logic

    Important design rule:
    This file must stay as a controller only.
    It should not contain engineering formulas or file-format logic.
*/

#include "../Core/SectionProjectIO.hpp"
#include "../Core/SectionExport.hpp"
#include "../Core/PropertyCalculator.hpp"

#include "FEMeshTab.hpp"
#include "GeneralTab.hpp"
#include "StressPointsTab.hpp"
#include "MeshDisplayWidget.hpp"
#include "MeshGenerator.hpp"

#include <QAction>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QStatusBar>
#include <QTabWidget>
#include <QWidget>
#include <QMessageBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_tabWidget(new QTabWidget(this))
{
    // Create the three main tabs required by the specification.
    m_tabWidget->addTab(createGeneralTab(), tr("General"));
    m_tabWidget->addTab(createStressPointsTab(), tr("Stress Points"));
    m_tabWidget->addTab(createFEMeshTab(), tr("FE Mesh"));

    // The General tab is the source of truth for the current section input.
    // Once the input changes, we store it and update the dependent tabs.
    if (m_generalTab != nullptr)
    {
        m_currentInput = m_generalTab->currentInput();

        // When the user edits dimensions in the General tab,
        // this callback updates the stored input and refreshes the rest of the UI.
        m_generalTab->setInputChangedCallback([this](const SectionInput& input) {
            handleSectionInputChanged(input);
        });

        refreshDependentTabs();
    }

    // File menu for project save/load/export actions.
    // The menu only triggers backend functions; it does not implement them.
    auto* fileMenu = menuBar()->addMenu(tr("&File"));

    // Save Project:
    // Stores the current section input in a project file.
    m_saveProjectAction = fileMenu->addAction(
        tr("&Save Project..."),
        this,
        &MainWindow::onSaveProjectTriggered,
        QKeySequence::Save);

    // Load Project:
    // Reads a project file and restores the section input into the GUI.
    m_loadProjectAction = fileMenu->addAction(
        tr("&Load Project..."),
        this,
        &MainWindow::onLoadProjectTriggered,
        QKeySequence::Open);

    // A separator keeps project actions and export actions visually distinct.
    fileMenu->addSeparator();

    // Export Results:
    // Produces output files from the current section and calculated properties.
    m_exportResultsAction = fileMenu->addAction(
        tr("&Export Results..."),
        this,
        &MainWindow::onExportResultsTriggered);

    // Finish window setup.
    setCentralWidget(m_tabWidget);
    setWindowTitle(tr("Beam Element Section Property Tool"));
    resize(1100, 700);
}

GeneralTab* MainWindow::createGeneralTab()
{
    // General tab is where the section input is edited.
    // We keep the pointer so we can later push loaded values back into it.
    m_generalTab = new GeneralTab(m_tabWidget);
    return m_generalTab;
}

StressPointsTab* MainWindow::createStressPointsTab()
{
    // Stress Points tab shows the default or edited stress output points.
    m_stressPointsTab = new StressPointsTab(m_tabWidget);
    m_stressPointsTab->setObjectName(QStringLiteral("stressPointsTab"));
    return m_stressPointsTab;
}

FEMeshTab* MainWindow::createFEMeshTab()
{
    // FE Mesh tab handles mesh-size selection and mesh generation requests.
    m_feMeshTab = new FEMeshTab(m_tabWidget);
    m_feMeshTab->setObjectName(QStringLiteral("feMeshTab"));

    // This widget is the visual host for mesh display.
    // The mesh generator fills it with generated mesh data.
    m_meshDisplayWidget = new MeshDisplayWidget(m_feMeshTab);
    m_feMeshTab->setExternalMeshWidget(m_meshDisplayWidget);

    // Connect the Generate button to the mesh generation slot.
    connect(m_feMeshTab->generateButton(), &QPushButton::clicked,
            this, &MainWindow::onGenerateMeshClicked);

    // Keep the mesh controls enabled because the module is available.
    // If the module were missing, this could be disabled instead.
    m_feMeshTab->setMeshControlsEnabled(true);

    return m_feMeshTab;
}

void MainWindow::handleSectionInputChanged(const SectionInput& input)
{
    // Store the latest input from the General tab.
    // This is the value used by Stress Points, FE Mesh, and export actions.
    m_currentInput = input;

    // Keep dependent tabs in sync.
    refreshDependentTabs();
}

void MainWindow::refreshDependentTabs()
{
    // Stress Points tab depends on the current section input.
    if (m_stressPointsTab != nullptr)
    {
        m_stressPointsTab->setSectionInput(m_currentInput);
    }

    // FE Mesh tab also depends on the current section input.
    if (m_feMeshTab != nullptr)
    {
        m_feMeshTab->setSectionInput(m_currentInput);
    }
}

SectionProperties MainWindow::calculateCurrentProperties() const
{
    // The GUI does not know the formulas.
    // It only chooses the correct backend routine based on section type.
    switch (m_currentInput.type)
    {
    case SectionType::HSection:
        return PropertyCalculator::calculateHSectionProperties(m_currentInput);

    case SectionType::BoxSection:
        return PropertyCalculator::calculateBoxSectionProperties(m_currentInput);

    case SectionType::PipeSection:
        return PropertyCalculator::calculatePipeSectionProperties(m_currentInput);

    case SectionType::GirderSection:
        return PropertyCalculator::calculateGirderSectionProperties(m_currentInput);
    }

    // Safe fallback if the enum ever becomes invalid.
    return {};
}

void MainWindow::onGenerateMeshClicked()
{
    // Mesh generation requires:
    // 1. a valid FE Mesh tab
    // 2. a valid mesh display widget
    // 3. a valid current section shape from the General tab
    if (!m_feMeshTab || !m_meshDisplayWidget || !m_generalTab)
    {
        return;
    }

    // Ask the General tab for the current geometric shape.
    // The mesh generator works on geometry, not on raw input fields.
    SectionShape shape = m_generalTab->getCurrentShape();

    // If the shape is empty, the section is not valid enough for meshing.
    if (shape.boundaryPoints.empty())
    {
        QMessageBox::warning(
            this,
            tr("Mesh Generation"),
            tr("No valid section shape available.\nPlease check General tab inputs."));
        return;
    }

    // Read the requested mesh size from the FE Mesh tab.
    double meshSize = m_feMeshTab->requestedMeshSize();

    // Use a reasonable default if the user entered an invalid value.
    if (meshSize <= 0.0)
    {
        meshSize = 100.0;
    }

    // Generate the mesh from the shape and size.
    MeshData meshData = MeshGenerator::generate(shape, meshSize);

    // If generation succeeded, show it.
    if (meshData.isValid && !meshData.triangles.empty())
    {
        m_meshDisplayWidget->setMeshData(meshData);
    }
    else
    {
        // Otherwise clear the widget and notify the user.
        QMessageBox::warning(
            this,
            tr("Mesh Generation"),
            tr("Failed to generate mesh.\nPlease check the section geometry."));
        m_meshDisplayWidget->clear();
    }
}

void MainWindow::onSaveProjectTriggered()
{
    // Ask the user where the project file should be saved.
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Save Project"),
        QString(),
        tr("Project Files (*.txt);;All Files (*)"));

    // If the user cancels, do nothing.
    if (filePath.isEmpty())
    {
        return;
    }

    // Project save options.
    // Precision 2 matches the assignment requirement of two decimal places.
    ExportOptions options;
    options.precision = 2;

    // Save the current section input to disk.
    // If your backend save function has a different signature,
    // adjust this line to match it.
    const bool ok = SectionProjectIO::saveSectionProject(
        m_currentInput,
        filePath.toStdString(),
        options);

    if (ok)
    {
        statusBar()->showMessage(tr("Project saved successfully."), 3000);
    }
    else
    {
        QMessageBox::warning(
            this,
            tr("Save Project"),
            tr("Failed to save the project file."));
    }
}

void MainWindow::onLoadProjectTriggered()
{
    // Ask the user which project file should be loaded.
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Load Project"),
        QString(),
        tr("Project Files (*.txt);;All Files (*)"));

    // If the user cancels, do nothing.
    if (filePath.isEmpty())
    {
        return;
    }

    // Temporary object used to receive the loaded values.
    SectionInput loadedInput;

    // Load the section input from the selected file.
    if (!SectionProjectIO::loadSectionProject(filePath.toStdString(), loadedInput))
    {
        QMessageBox::warning(
            this,
            tr("Load Project"),
            tr("Failed to load the project file."));
        return;
    }

    // Replace the current input with the loaded data.
    m_currentInput = loadedInput;

    // Push the loaded input back into the General tab so the GUI refreshes.
    if (m_generalTab != nullptr)
    {
        m_generalTab->setSectionInput(m_currentInput);
    }

    // Refresh the tabs that depend on the section input.
    refreshDependentTabs();

    statusBar()->showMessage(tr("Project loaded successfully."), 3000);
}

void MainWindow::onExportResultsTriggered()
{
    // Ask the user for a base file name.
    // The code below will create separate report and FEM card files from it.
    const QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Export Results"),
        QString(),
        tr("Text Files (*.txt);;All Files (*)"));

    if (filePath.isEmpty())
    {
        return;
    }

    // Build output file names from the selected base path.
    const QFileInfo info(filePath);
    const QString reportPath =
        info.absolutePath() + "/" + info.completeBaseName() + "_report.txt";

    const QString femCardPath =
        info.absolutePath() + "/" + info.completeBaseName() + "_fem_card.txt";

    // Calculate the current section properties before export.
    const SectionProperties properties = calculateCurrentProperties();

    // Export a human-readable property report.
    const bool reportOk = SectionExport::writePropertiesReportToFile(
        m_currentInput,
        properties,
        reportPath.toStdString());

    // Export the FEM-style text card.
    const bool femOk = SectionExport::writeFemCardToFile(
        m_currentInput,
        properties,
        femCardPath.toStdString());

    // Report success only if both files were created.
    if (reportOk && femOk)
    {
        statusBar()->showMessage(tr("Results exported successfully."), 3000);
    }
    else
    {
        QMessageBox::warning(
            this,
            tr("Export Results"),
            tr("Failed to export one or more output files."));
    }
}