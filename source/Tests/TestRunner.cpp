#include <iostream>
#include <cmath>
#include <string>
#include "../Core/PropertyCalculator.hpp"

int failed_tests = 0;
int passed_tests = 0;

void ASSERT_NEAR(double expected, double actual, double tolerance, const std::string& testName) {
    if (std::abs(expected - actual) > tolerance) {
        std::cerr << "[FAIL] " << testName << " | Expected: " << expected << ", Actual: " << actual << std::endl;
        failed_tests++;
    } else {
        std::cout << "[PASS] " << testName << std::endl;
        passed_tests++;
    }
}

void TestHSection() {
    SectionInput input;
    input.type = SectionType::HSection;
    // Map from Test Data: A=100, H=210 (inner web height), e=20 (flange thickness), f=12 (web thickness)
    input.width = 100.0;
    input.height = 250.0; // 210 + 2*20
    input.flangeThickness = 20.0;
    input.webThickness = 12.0;

    SectionProperties props = PropertyCalculator::calculateHSectionProperties(input);

    std::cout << "--- Testing H Section ---" << std::endl;
    ASSERT_NEAR(6520.0, props.Area, 1e-1, "H-Section Area");
    // Reference Jz: 3.3636E+06
    ASSERT_NEAR(3.3636e6, props.Jz, 1000.0, "H-Section Jz"); 
    // Reference Jy: 6.2294E+07
    ASSERT_NEAR(6.2294e7, props.Jy, 1000.0, "H-Section Jy");
}

void TestPipeSection() {
    SectionInput input;
    input.type = SectionType::PipeSection;
    input.diameter = 1300.0;
    input.wallThickness = 14.0;

    SectionProperties props = PropertyCalculator::calculatePipeSectionProperties(input);

    std::cout << "--- Testing Pipe Section ---" << std::endl;
    // Reference Area: 5.6561E+04
    ASSERT_NEAR(5.6561e4, props.Area, 10.0, "Pipe-Section Area");
    // Reference Jy: 1.1694E+10
    ASSERT_NEAR(1.1694e10, props.Jy, 1e6, "Pipe-Section Jy");
    // Reference Jx: 2.3388E+10
    ASSERT_NEAR(2.3388e10, props.Jx, 1e6, "Pipe-Section Jx");
}

void TestBoxSection() {
    SectionInput input;
    input.type = SectionType::BoxSection;
    input.width = 1320.0;
    input.height = 2600.0;
    
    // Testing our robust asymmetric Box math
    input.topFlangeThickness = 40.0;
    input.bottomFlangeThickness = 22.0;
    input.leftWebThickness = 16.0;
    input.rightWebThickness = 16.0;

    SectionProperties props = PropertyCalculator::calculateBoxSectionProperties(input);

    std::cout << "--- Testing Box Section ---" << std::endl;
    // Expected Area calculation for robust internal check:
    // Inner dimensions: 1320 - 32 = 1288, 2600 - 62 = 2538
    // Area = (1320*2600) - (1288*2538) = 163056
    ASSERT_NEAR(163056.0, props.Area, 1e-1, "Box-Section Area (Internal Math Check)");
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Property Calculator Test Runner      " << std::endl;
    std::cout << "========================================" << std::endl;

    TestHSection();
    TestPipeSection();
    TestBoxSection();

    std::cout << "========================================" << std::endl;
    if (failed_tests == 0) {
        std::cout << "ALL TESTS PASSED (" << passed_tests << " assertions)" << std::endl;
    } else {
        std::cerr << failed_tests << " TESTS FAILED!" << std::endl;
        return 1;
    }
    
    return 0;
}
