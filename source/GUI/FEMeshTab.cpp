#include "FEMeshTab.hpp"

#include "SectionState.hpp"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
    QString sectionTypeName(SectionType type)
    {
        return QString::fromLatin1(toString(type));
    }
}

FEMeshTab::FEMeshTab(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
    updateSectionSummary();
    updateStatus();
}

void FEMeshTab::setSectionInput(const SectionInput& input)
{
    m_input = input;
    updateSectionSummary();
    updateStatus();
}

const SectionInput& FEMeshTab::sectionInput() const
{
    return m_input;
}

double FEMeshTab::requestedMeshSize() const
{
    return m_meshSizeSpinBox != nullptr ? m_meshSizeSpinBox->value() : 0.0;
}

void FEMeshTab::setMeshControlsEnabled(bool enabled)
{
    if (m_meshSizeSpinBox != nullptr)
    {
        m_meshSizeSpinBox->setEnabled(enabled);
    }

    if (m_generateButton != nullptr)
    {
        m_generateButton->setEnabled(enabled);
    }
}

QPushButton* FEMeshTab::generateButton() const
{
    return m_generateButton;
}

QWidget* FEMeshTab::visualizationHost() const
{
    return m_visualizationHost;
}

void FEMeshTab::setExternalMeshWidget(QWidget* widget)
{
    if (m_visualizationHostLayout == nullptr)
    {
        return;
    }

    clearExternalMeshWidget();
    m_externalMeshWidget = widget;

    if (m_externalMeshWidget != nullptr)
    {
        m_externalMeshWidget->setParent(m_visualizationHost);
        m_visualizationHostLayout->addWidget(m_externalMeshWidget);
        if (m_placeholderLabel != nullptr)
        {
            m_placeholderLabel->setVisible(false);
        }
    }
}

void FEMeshTab::clearExternalMeshWidget()
{
    if (m_externalMeshWidget == nullptr)
    {
        if (m_placeholderLabel != nullptr)
        {
            m_placeholderLabel->setVisible(true);
        }
        return;
    }

    if (m_visualizationHostLayout != nullptr)
    {
        m_visualizationHostLayout->removeWidget(m_externalMeshWidget);
    }
    m_externalMeshWidget->setParent(nullptr);
    m_externalMeshWidget = nullptr;

    if (m_placeholderLabel != nullptr)
    {
        m_placeholderLabel->setVisible(true);
    }
}

void FEMeshTab::setupUi()
{
    auto* rootLayout = new QHBoxLayout(this);

    auto* controlsPanel = new QWidget(this);
    auto* controlsLayout = new QVBoxLayout(controlsPanel);

    auto* sectionGroup = new QGroupBox(QStringLiteral("Section"), controlsPanel);
    auto* sectionLayout = new QVBoxLayout(sectionGroup);
    m_sectionSummaryLabel = new QLabel(sectionGroup);
    sectionLayout->addWidget(m_sectionSummaryLabel);

    auto* meshGroup = new QGroupBox(QStringLiteral("Mesh Controls"), controlsPanel);
    auto* meshLayout = new QVBoxLayout(meshGroup);
    auto* meshForm = new QFormLayout();

    m_meshSizeSpinBox = new QDoubleSpinBox(meshGroup);
    m_meshSizeSpinBox->setRange(1.0, 1000000.0);
    m_meshSizeSpinBox->setDecimals(2);
    m_meshSizeSpinBox->setSingleStep(10.0);
    m_meshSizeSpinBox->setValue(100.0);
    m_meshSizeSpinBox->setSuffix(QStringLiteral(" mm"));

    meshForm->addRow(QStringLiteral("Mesh Size"), m_meshSizeSpinBox);
    meshLayout->addLayout(meshForm);

    m_generateButton = new QPushButton(QStringLiteral("Generate Mesh"), meshGroup);
    meshLayout->addWidget(m_generateButton);

    m_statusLabel = new QLabel(meshGroup);
    m_statusLabel->setWordWrap(true);
    m_statusLabel->setStyleSheet(QStringLiteral("color: #6f7682;"));
    meshLayout->addWidget(m_statusLabel);
    meshLayout->addStretch(1);

    controlsLayout->addWidget(sectionGroup);
    controlsLayout->addWidget(meshGroup, 1);

    auto* visualizationGroup = new QGroupBox(QStringLiteral("Mesh Visualization"), this);
    auto* visualizationLayout = new QVBoxLayout(visualizationGroup);

    auto* visualizationFrame = new QFrame(visualizationGroup);
    visualizationFrame->setObjectName(QStringLiteral("meshVisualizationHost"));
    visualizationFrame->setFrameShape(QFrame::StyledPanel);
    visualizationFrame->setMinimumSize(420, 320);
    visualizationFrame->setStyleSheet(QStringLiteral(
        "QFrame#meshVisualizationHost { background: #fafbfc; border: 1px solid #d0d7de; }"));

    m_visualizationHostLayout = new QVBoxLayout(visualizationFrame);
    m_placeholderLabel = new QLabel(QStringLiteral("Mesh visualization pending"), visualizationFrame);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet(QStringLiteral("color: #6f7682;"));
    m_visualizationHostLayout->addWidget(m_placeholderLabel);

    visualizationLayout->addWidget(visualizationFrame, 1);
    m_visualizationHost = visualizationFrame;

    rootLayout->addWidget(controlsPanel, 0);
    rootLayout->addWidget(visualizationGroup, 1);

    setMeshControlsEnabled(false);
}

void FEMeshTab::updateSectionSummary()
{
    if (m_sectionSummaryLabel != nullptr)
    {
        m_sectionSummaryLabel->setText(QStringLiteral("Current section: %1")
                                           .arg(sectionTypeName(m_input.type)));
    }
}

void FEMeshTab::updateStatus()
{
    if (m_statusLabel == nullptr)
    {
        return;
    }

    const ValidationResult validation = SectionState::validateInput(m_input);
    if (!validation.valid)
    {
        m_statusLabel->setText(QStringLiteral("General tab input must be valid before meshing."));
        m_statusLabel->setStyleSheet(QStringLiteral("color: #b3261e;"));
        return;
    }

    m_statusLabel->setText(QStringLiteral("Mesh module unavailable. Controls are placeholders."));
    m_statusLabel->setStyleSheet(QStringLiteral("color: #6f7682;"));
}
