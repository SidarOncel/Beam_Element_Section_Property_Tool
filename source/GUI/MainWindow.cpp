#include "MainWindow.hpp"

#include "FEMeshTab.hpp"
#include "GeneralTab.hpp"
#include "StressPointsTab.hpp"

#include <QTabWidget>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      m_tabWidget(new QTabWidget(this))
{
    m_tabWidget->addTab(createGeneralTab(), tr("General"));
    m_tabWidget->addTab(createStressPointsTab(), tr("Stress Points"));
    m_tabWidget->addTab(createFEMeshTab(), tr("FE Mesh"));

    if (m_generalTab != nullptr)
    {
        m_currentInput = m_generalTab->currentInput();
        m_generalTab->setInputChangedCallback([this](const SectionInput& input) {
            handleSectionInputChanged(input);
        });
        refreshDependentTabs();
    }

    setCentralWidget(m_tabWidget);
    setWindowTitle(tr("Beam Element Section Property Tool"));
    resize(1100, 700);
}

GeneralTab* MainWindow::createGeneralTab()
{
    m_generalTab = new GeneralTab(m_tabWidget);
    return m_generalTab;
}

StressPointsTab* MainWindow::createStressPointsTab()
{
    m_stressPointsTab = new StressPointsTab(m_tabWidget);
    m_stressPointsTab->setObjectName(QStringLiteral("stressPointsTab"));
    return m_stressPointsTab;
}

FEMeshTab* MainWindow::createFEMeshTab()
{
    m_feMeshTab = new FEMeshTab(m_tabWidget);
    m_feMeshTab->setObjectName(QStringLiteral("feMeshTab"));
    return m_feMeshTab;
}

void MainWindow::handleSectionInputChanged(const SectionInput& input)
{
    m_currentInput = input;
    refreshDependentTabs();
}

void MainWindow::refreshDependentTabs()
{
    if (m_stressPointsTab != nullptr)
    {
        m_stressPointsTab->setSectionInput(m_currentInput);
    }

    if (m_feMeshTab != nullptr)
    {
        m_feMeshTab->setSectionInput(m_currentInput);
    }
}
