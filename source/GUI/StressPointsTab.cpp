#include "StressPointsTab.hpp"

#include "PreviewShapeAdapter.hpp"
#include "SectionPreviewWidget.hpp"
#include "SectionState.hpp"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace
{
    constexpr int kPointColumn = 0;
    constexpr int kYColumn = 1;
    constexpr int kZColumn = 2;

    QString coordinateText(double value)
    {
        return QString::number(value, 'f', 2);
    }

    QTableWidgetItem* readOnlyItem(const QString& text)
    {
        auto* item = new QTableWidgetItem(text);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignCenter);
        return item;
    }

    QTableWidgetItem* editableCoordinateItem(double value)
    {
        auto* item = new QTableWidgetItem(coordinateText(value));
        item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        return item;
    }

    QString validationText(const ValidationResult& validation)
    {
        QStringList lines;
        for (const std::string& error : validation.errors)
        {
            lines.push_back(QString::fromStdString(error));
        }
        return lines.join(QLatin1Char('\n'));
    }
}

StressPointsTab::StressPointsTab(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void StressPointsTab::setSectionInput(const SectionInput& input)
{
    m_input = input;
    resetToDefaultStressPoints();
}

const SectionInput& StressPointsTab::sectionInput() const
{
    return m_input;
}

std::vector<StressPoint> StressPointsTab::stressPoints() const
{
    return m_points;
}

void StressPointsTab::setupUi()
{
    auto* rootLayout = new QHBoxLayout(this);

    auto* inputPanel = new QWidget(this);
    auto* inputLayout = new QVBoxLayout(inputPanel);

    auto* tableLabel = new QLabel(QStringLiteral("Stress Output Points"), inputPanel);
    m_table = new QTableWidget(inputPanel);
    m_table->setColumnCount(3);
    m_table->setRowCount(4);
    m_table->setHorizontalHeaderLabels({
        QStringLiteral("Point"),
        QStringLiteral("y (mm)"),
        QStringLiteral("z (mm)")
    });
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->horizontalHeader()->setSectionResizeMode(kPointColumn, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(kYColumn, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(kZColumn, QHeaderView::Stretch);

    auto* resetButton = new QPushButton(QStringLiteral("Reset to Defaults"), inputPanel);

    inputLayout->addWidget(tableLabel);
    inputLayout->addWidget(m_table, 1);
    inputLayout->addWidget(resetButton);

    m_statusLabel = new QLabel(inputPanel);
    m_statusLabel->setWordWrap(true);
    inputLayout->addWidget(m_statusLabel);

    auto* previewPanel = new QWidget(this);
    auto* previewLayout = new QVBoxLayout(previewPanel);
    auto* previewLabel = new QLabel(QStringLiteral("Section Preview"), previewPanel);
    m_previewWidget = new SectionPreviewWidget(previewPanel);
    previewLayout->addWidget(previewLabel);
    previewLayout->addWidget(m_previewWidget, 1);

    rootLayout->addWidget(inputPanel, 0);
    rootLayout->addWidget(previewPanel, 1);

    connect(m_table,
            &QTableWidget::cellChanged,
            this,
            [this](int row, int column) { handleCellChanged(row, column); });

    connect(resetButton,
            &QPushButton::clicked,
            this,
            [this]() { resetToDefaultStressPoints(); });
}

void StressPointsTab::resetToDefaultStressPoints()
{
    m_points = StressPointDefaults::defaultStressPoints(m_input);
    updateTableFromPoints();
    updatePreview();
}

void StressPointsTab::updateTableFromPoints()
{
    m_isUpdating = true;
    m_table->setRowCount(static_cast<int>(m_points.size()));

    for (int row = 0; row < static_cast<int>(m_points.size()); ++row)
    {
        const StressPoint& point = m_points[static_cast<std::size_t>(row)];
        m_table->setItem(row, kPointColumn, readOnlyItem(QString::number(point.id)));
        m_table->setItem(row, kYColumn, editableCoordinateItem(point.coordinates.y));
        m_table->setItem(row, kZColumn, editableCoordinateItem(point.coordinates.z));
    }

    m_isUpdating = false;
}

void StressPointsTab::updatePreview()
{
    const ValidationResult validation = SectionState::validateInput(m_input);
    if (!validation.valid)
    {
        m_previewWidget->clearShape();
        setStatusMessage(QStringLiteral("General tab input is invalid:\n%1")
                             .arg(validationText(validation)),
                         true);
        return;
    }

    m_previewWidget->setShape(PreviewShapeAdapter::createShape(m_input));

    std::vector<PreviewPointMarker> markers;
    const std::vector<StressPoint> previewPoints =
        StressPointDefaults::previewStressPoints(m_input, m_points);
    markers.reserve(previewPoints.size());
    for (const StressPoint& point : previewPoints)
    {
        markers.push_back({QString::number(point.id), point.coordinates});
    }

    m_previewWidget->setPointMarkers(markers);
    setStatusMessage(QStringLiteral("Stress point coordinates are editable in mm."), false);
}

void StressPointsTab::setStatusMessage(const QString& message, bool isError)
{
    if (m_statusLabel == nullptr)
    {
        return;
    }

    m_statusLabel->setText(message);
    m_statusLabel->setStyleSheet(isError ? QStringLiteral("color: #b3261e;")
                                         : QStringLiteral("color: #4f5965;"));
}

void StressPointsTab::handleCellChanged(int row, int column)
{
    if (m_isUpdating || row < 0 || row >= static_cast<int>(m_points.size()) ||
        (column != kYColumn && column != kZColumn))
    {
        return;
    }

    QTableWidgetItem* item = m_table->item(row, column);
    if (item == nullptr)
    {
        return;
    }

    bool ok = false;
    const double value = item->text().toDouble(&ok);
    StressPoint& point = m_points[static_cast<std::size_t>(row)];
    double& coordinate = column == kYColumn ? point.coordinates.y : point.coordinates.z;

    if (!ok)
    {
        m_isUpdating = true;
        item->setText(coordinateText(coordinate));
        m_isUpdating = false;
        setStatusMessage(QStringLiteral("Enter numeric stress point coordinates in mm."), true);
        return;
    }

    coordinate = value;
    m_isUpdating = true;
    item->setText(coordinateText(coordinate));
    m_isUpdating = false;
    updatePreview();
}
