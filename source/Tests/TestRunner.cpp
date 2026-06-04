#include <iostream>
#include <cmath>
#include <string>
#include "../Core/PropertyCalculator.hpp"

int failed_tests = 0;
int passed_tests = 0;

void ASSERT_NEAR(double expected, double actual, double tolerance, const std::string& testName) {
    if (std::abs(expected - actual) > tolerance) {
        std::cout << "[FAIL] " << testName << " | Expected: " << expected << ", Actual: " << actual << std::endl;
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

void TestGirderSection() {
    SectionInput input;
    input.type = SectionType::GirderSection;
    
    // Test Data parameters
    input.A = 766.0; input.B = 836.0; input.G = 1090.0; input.D = 1160.0;
    input.e = 20.0; input.f = 12.0; input.H = 2000.0; input.W = 934.0;
    input.M = 350.0; input.N = 518.0; input.p = 20.0; input.s = 12.0;
    input.t = 10.0; input.u = 10.0; input.M1 = 175.0; input.k = 150.0;
    input.k1 = 12.0; input.h = 138.0; input.h1 = 10.0;

    SectionProperties props = PropertyCalculator::calculateGirderSectionProperties(input);

    std::cout << "--- Testing Quayside Crane Girder ---" << std::endl;
    ASSERT_NEAR(8.7174e4, props.Area, 50.0, "Girder Area");
    ASSERT_NEAR(1.4889e10, props.Jz, 1e8, "Girder Jz");
    ASSERT_NEAR(4.9105e10, props.Jy, 1e8, "Girder Jy");
    ASSERT_NEAR(2.1432e10, props.Jx, 1e8, "Girder Jx");
    ASSERT_NEAR(-8.0260e9, props.Jyz, 1e8, "Girder Jyz");
    ASSERT_NEAR(1.3099e10, props.Jzo, 1e8, "Girder Jzo");
    ASSERT_NEAR(5.0894e10, props.Jyo, 1e8, "Girder Jyo");
    // Depending on origin choices, cy and cz might be shifted in Test_data. 
    // Usually they are relative to an arbitrary origin (like bottom left). 
    // We will just verify Area and inertias to confirm the math logic.
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Property Calculator Test Runner      " << std::endl;
    std::cout << "========================================" << std::endl;

    TestHSection();
    TestPipeSection();
    TestBoxSection();
    TestGirderSection();

    std::cout << "========================================" << std::endl;
    if (failed_tests == 0) {
        std::cout << "ALL TESTS PASSED (" << passed_tests << " assertions)" << std::endl;
    } else {
        std::cout << failed_tests << " TESTS FAILED!" << std::endl;
        return 1;
    }
    
    return 0;
}
