#pragma once

#include "SectionState.hpp"
#include "../Geometry/SectionShape.hpp"

#include <QWidget>

#include <functional>
#include <vector>

class QComboBox;
class QDoubleSpinBox;
class QFormLayout;
class QLabel;
class QTableWidget;
class SectionPreviewWidget;

class GeneralTab : public QWidget
{
public:
    explicit GeneralTab(QWidget* parent = nullptr);

    const SectionInput& currentInput() const;
    void setInputChangedCallback(std::function<void(const SectionInput&)> callback);
    void setSectionInput(const SectionInput& input);

    SectionShape getCurrentShape() const;

private:
    struct ParameterBinding
    {
        QString label;
        double SectionInput::* member = nullptr;
        QDoubleSpinBox* editor = nullptr;
    };

    void setupUi();
    void rebuildParameterForm();
    void addParameterField(const QString& label, double SectionInput::* member);
    void clearParameterForm();
    void handleSectionTypeChanged();
    void updateWorkflow();
    void updatePropertyTable();
    void updateValidationDisplay();

    SectionType currentSectionType() const;
    SectionInput defaultInputForType(SectionType type) const;
    SectionInput inputFromForm() const;

    SectionState m_state;
    QComboBox* m_sectionTypeCombo = nullptr;
    QFormLayout* m_parameterForm = nullptr;
    QLabel* m_validationLabel = nullptr;
    QTableWidget* m_propertyTable = nullptr;
    SectionPreviewWidget* m_previewWidget = nullptr;
    std::vector<ParameterBinding> m_parameterBindings;
    std::function<void(const SectionInput&)> m_inputChangedCallback;
    bool m_isUpdating = false;
};