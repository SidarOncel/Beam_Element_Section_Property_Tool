#include "MainWindow.hpp"

#include "FEMeshTab.hpp"
#include "GeneralTab.hpp"
#include "StressPointsTab.hpp"
#include "MeshDisplayWidget.hpp"
#include "MeshGenerator.hpp"

#include <QTabWidget>
#include <QWidget>
#include <QMessageBox>
#include <QPushButton>

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
    
    // Mesh display widget'ı oluştur ve FE Mesh tab'ının içine yerleştir
    m_meshDisplayWidget = new MeshDisplayWidget(m_feMeshTab);
    m_feMeshTab->setExternalMeshWidget(m_meshDisplayWidget);
    
    // Generate butonuna tıklama olayını bağla
    connect(m_feMeshTab->generateButton(), &QPushButton::clicked,
            this, &MainWindow::onGenerateMeshClicked);
    
    // Mesh kontrollerini aktif et (placeholder değil, gerçek modül)
    m_feMeshTab->setMeshControlsEnabled(true);
    
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

void MainWindow::onGenerateMeshClicked()
{
    if (!m_feMeshTab || !m_meshDisplayWidget || !m_generalTab)
    {
        return;
    }
    
    // GeneralTab'dan mevcut şekli al
    SectionShape shape = m_generalTab->getCurrentShape();
    
    if (shape.boundaryPoints.empty())
    {
        QMessageBox::warning(this, tr("Mesh Generation"),
                            tr("No valid section shape available.\nPlease check General tab inputs."));
        return;
    }
    
    // Mesh boyutunu al
    double meshSize = m_feMeshTab->requestedMeshSize();
    if (meshSize <= 0.0)
    {
        meshSize = 100.0;  // Varsayılan değer
    }
    
    // Mesh'i oluştur
    MeshData meshData = MeshGenerator::generate(shape, meshSize);
    
    if (meshData.isValid && !meshData.triangles.empty())
    {
        m_meshDisplayWidget->setMeshData(meshData);
    }
    else
    {
        QMessageBox::warning(this, tr("Mesh Generation"),
                            tr("Failed to generate mesh.\nPlease check the section geometry."));
        m_meshDisplayWidget->clear();
    }
}