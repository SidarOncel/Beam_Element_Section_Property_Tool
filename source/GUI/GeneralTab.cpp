#include "GeneralTab.hpp"

#include "PreviewShapeAdapter.hpp"
#include "SectionPreviewWidget.hpp"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QScrollArea>
#include <QSplitter>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QVariant>

#include <utility>

namespace
{
    QString sectionTypeName(SectionType type)
    {
        return QString::fromLatin1(toString(type));
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

GeneralTab::GeneralTab(QWidget* parent)
    : QWidget(parent)
{
    setupUi();

    m_state.setInput(defaultInputForType(SectionType::HSection));
    rebuildParameterForm();
    updateWorkflow();
}

const SectionInput& GeneralTab::currentInput() const
{
    return m_state.input();
}

void GeneralTab::setInputChangedCallback(std::function<void(const SectionInput&)> callback)
{
    m_inputChangedCallback = std::move(callback);
}

SectionShape GeneralTab::getCurrentShape() const
{
    return PreviewShapeAdapter::createShape(m_state.input());
}

void GeneralTab::setupUi()
{
    auto* rootLayout = new QHBoxLayout(this);
    auto* splitter = new QSplitter(Qt::Horizontal, this);
    rootLayout->addWidget(splitter);

    auto* inputPanel = new QWidget(splitter);
    auto* inputLayout = new QVBoxLayout(inputPanel);

    auto* sectionLabel = new QLabel(QStringLiteral("Section Type"), inputPanel);
    m_sectionTypeCombo = new QComboBox(inputPanel);
    m_sectionTypeCombo->addItem(sectionTypeName(SectionType::HSection),
                                static_cast<int>(SectionType::HSection));
    m_sectionTypeCombo->addItem(sectionTypeName(SectionType::BoxSection),
                                static_cast<int>(SectionType::BoxSection));
    m_sectionTypeCombo->addItem(sectionTypeName(SectionType::PipeSection),
                                static_cast<int>(SectionType::PipeSection));
    m_sectionTypeCombo->addItem(sectionTypeName(SectionType::GirderSection),
                                static_cast<int>(SectionType::GirderSection));

    inputLayout->addWidget(sectionLabel);
    inputLayout->addWidget(m_sectionTypeCombo);

    auto* formContainer = new QWidget(inputPanel);
    m_parameterForm = new QFormLayout(formContainer);
    m_parameterForm->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    auto* scrollArea = new QScrollArea(inputPanel);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(formContainer);
    inputLayout->addWidget(scrollArea, 1);

    m_validationLabel = new QLabel(inputPanel);
    m_validationLabel->setWordWrap(true);
    m_validationLabel->setStyleSheet(QStringLiteral("color: #b3261e;"));
    inputLayout->addWidget(m_validationLabel);

    auto* outputPanel = new QWidget(splitter);
    auto* outputLayout = new QVBoxLayout(outputPanel);

    auto* previewLabel = new QLabel(QStringLiteral("Section Preview"), outputPanel);
    m_previewWidget = new SectionPreviewWidget(outputPanel);
    outputLayout->addWidget(previewLabel);
    outputLayout->addWidget(m_previewWidget, 3);

    auto* tableLabel = new QLabel(QStringLiteral("Section Properties"), outputPanel);
    m_propertyTable = new QTableWidget(outputPanel);
    m_propertyTable->setColumnCount(3);
    m_propertyTable->setHorizontalHeaderLabels({
        QStringLiteral("Property"),
        QStringLiteral("Value"),
        QStringLiteral("Unit")
    });
    m_propertyTable->horizontalHeader()->setStretchLastSection(true);
    m_propertyTable->verticalHeader()->setVisible(false);
    m_propertyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_propertyTable->setSelectionMode(QAbstractItemView::NoSelection);

    outputLayout->addWidget(tableLabel);
    outputLayout->addWidget(m_propertyTable, 2);

    splitter->addWidget(inputPanel);
    splitter->addWidget(outputPanel);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({320, 780});

    connect(m_sectionTypeCombo,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,
            [this](int) { handleSectionTypeChanged(); });
}

void GeneralTab::rebuildParameterForm()
{
    m_isUpdating = true;
    clearParameterForm();

    switch (m_state.input().type)
    {
    case SectionType::HSection:
        addParameterField(QStringLiteral("Width"), &SectionInput::width);
        addParameterField(QStringLiteral("Height"), &SectionInput::height);
        addParameterField(QStringLiteral("Flange Thickness"), &SectionInput::flangeThickness);
        addParameterField(QStringLiteral("Web Thickness"), &SectionInput::webThickness);
        break;
    case SectionType::BoxSection:
        addParameterField(QStringLiteral("Width"), &SectionInput::width);
        addParameterField(QStringLiteral("Height"), &SectionInput::height);
        addParameterField(QStringLiteral("Top Thickness"), &SectionInput::topFlangeThickness);
        addParameterField(QStringLiteral("Bottom Thickness"), &SectionInput::bottomFlangeThickness);
        addParameterField(QStringLiteral("Left Web Thickness"), &SectionInput::leftWebThickness);
        addParameterField(QStringLiteral("Right Web Thickness"), &SectionInput::rightWebThickness);
        break;
    case SectionType::PipeSection:
        addParameterField(QStringLiteral("Outer Diameter"), &SectionInput::diameter);
        addParameterField(QStringLiteral("Wall Thickness"), &SectionInput::wallThickness);
        break;
    case SectionType::GirderSection:
        addParameterField(QStringLiteral("A"), &SectionInput::A);
        addParameterField(QStringLiteral("B"), &SectionInput::B);
        addParameterField(QStringLiteral("G"), &SectionInput::G);
        addParameterField(QStringLiteral("D"), &SectionInput::D);
        addParameterField(QStringLiteral("e"), &SectionInput::e);
        addParameterField(QStringLiteral("f"), &SectionInput::f);
        addParameterField(QStringLiteral("H"), &SectionInput::H);
        addParameterField(QStringLiteral("W"), &SectionInput::W);
        addParameterField(QStringLiteral("M"), &SectionInput::M);
        addParameterField(QStringLiteral("N"), &SectionInput::N);
        addParameterField(QStringLiteral("p"), &SectionInput::p);
        addParameterField(QStringLiteral("s"), &SectionInput::s);
        addParameterField(QStringLiteral("t"), &SectionInput::t);
        addParameterField(QStringLiteral("u"), &SectionInput::u);
        addParameterField(QStringLiteral("M1"), &SectionInput::M1);
        addParameterField(QStringLiteral("k"), &SectionInput::k);
        addParameterField(QStringLiteral("k1"), &SectionInput::k1);
        addParameterField(QStringLiteral("h"), &SectionInput::h);
        addParameterField(QStringLiteral("h1"), &SectionInput::h1);
        break;
    }

    for (ParameterBinding& binding : m_parameterBindings)
    {
        binding.editor->setValue(m_state.input().*(binding.member));
    }

    m_isUpdating = false;
}

void GeneralTab::addParameterField(const QString& label, double SectionInput::* member)
{
    auto* editor = new QDoubleSpinBox(this);
    editor->setRange(0.0, 1000000000.0);
    editor->setDecimals(2);
    editor->setSingleStep(1.0);
    editor->setSuffix(QStringLiteral(" mm"));

    m_parameterForm->addRow(label, editor);
    m_parameterBindings.push_back({label, member, editor});

    connect(editor,
            static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
            this,
            [this](double) {
                if (!m_isUpdating)
                {
                    updateWorkflow();
                }
            });
}

void GeneralTab::clearParameterForm()
{
    while (QLayoutItem* item = m_parameterForm->takeAt(0))
    {
        if (QWidget* widget = item->widget())
        {
            widget->deleteLater();
        }
        delete item;
    }

    m_parameterBindings.clear();
}

void GeneralTab::handleSectionTypeChanged()
{
    if (m_isUpdating)
    {
        return;
    }

    m_state.setInput(defaultInputForType(currentSectionType()));
    rebuildParameterForm();
    updateWorkflow();
}

void GeneralTab::updateWorkflow()
{
    SectionInput input = inputFromForm();
    m_state.setInput(input);

    if (m_state.recalculate())
    {
        m_previewWidget->setShape(PreviewShapeAdapter::createShape(m_state.input()));
    }
    else
    {
        m_previewWidget->clearShape();
    }

    updateValidationDisplay();
    updatePropertyTable();

    if (m_inputChangedCallback)
    {
        m_inputChangedCallback(m_state.input());
    }
}

void GeneralTab::updatePropertyTable()
{
    const std::vector<PropertyDisplayRow> rows = m_state.formattedPropertyRows(2);
    m_propertyTable->setRowCount(static_cast<int>(rows.size()));

    for (int row = 0; row < static_cast<int>(rows.size()); ++row)
    {
        const PropertyDisplayRow& property = rows[static_cast<std::size_t>(row)];
        m_propertyTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(property.name)));
        m_propertyTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(property.value)));
        m_propertyTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(property.unit)));
    }

    m_propertyTable->resizeColumnsToContents();
}

void GeneralTab::updateValidationDisplay()
{
    const ValidationResult& validation = m_state.lastValidation();
    m_validationLabel->setVisible(!validation.valid);
    m_validationLabel->setText(validationText(validation));
}

SectionType GeneralTab::currentSectionType() const
{
    return static_cast<SectionType>(m_sectionTypeCombo->currentData().toInt());
}

SectionInput GeneralTab::defaultInputForType(SectionType type) const
{
    SectionInput input;
    input.type = type;

    switch (type)
    {
    case SectionType::HSection:
        input.width = 100.0;
        input.height = 250.0;
        input.flangeThickness = 20.0;
        input.webThickness = 12.0;
        break;
    case SectionType::BoxSection:
        input.width = 1320.0;
        input.height = 2600.0;
        input.topFlangeThickness = 40.0;
        input.bottomFlangeThickness = 22.0;
        input.leftWebThickness = 16.0;
        input.rightWebThickness = 16.0;
        break;
    case SectionType::PipeSection:
        input.diameter = 1300.0;
        input.wallThickness = 14.0;
        break;
    case SectionType::GirderSection:
        input.A = 766.0;
        input.B = 836.0;
        input.G = 1090.0;
        input.D = 1160.0;
        input.e = 20.0;
        input.f = 12.0;
        input.H = 2000.0;
        input.W = 934.0;
        input.M = 350.0;
        input.N = 518.0;
        input.p = 20.0;
        input.s = 12.0;
        input.t = 10.0;
        input.u = 10.0;
        input.M1 = 175.0;
        input.k = 150.0;
        input.k1 = 12.0;
        input.h = 138.0;
        input.h1 = 10.0;
        break;
    }

    return input;
}

SectionInput GeneralTab::inputFromForm() const
{
    SectionInput input = m_state.input();
    input.type = currentSectionType();

    for (const ParameterBinding& binding : m_parameterBindings)
    {
        input.*(binding.member) = binding.editor->value();
    }

    return input;
}