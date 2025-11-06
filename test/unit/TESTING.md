# Unit Test Implementation Guide

## Overview

This document provides detailed information about the unit tests implemented for k4ActsTracking, their purpose, coverage, and how to extend them.

## Test Architecture

### Testing Framework

We use **Google Test (gtest)** as the unit testing framework because:
- Industry standard for C++ testing
- Rich assertion macros
- Test fixture support for setup/teardown
- Good integration with CMake and CI systems
- Widely used in HEP community

### Test Organization

Tests are organized by component:
```
test/unit/
├── CMakeLists.txt           # Build configuration for unit tests
├── TestMocks.h              # Mock objects for testing
├── test_ActsGaudiLogger.cpp # Tests for logger integration
├── test_Helpers.cpp         # Tests for utility functions
└── test_Geometry.cpp        # Tests for geometry context
```

## Implemented Tests

### 1. ActsGaudiLogger Tests (`test_ActsGaudiLogger.cpp`)

**Purpose**: Validate the integration layer between Acts logging and Gaudi framework.

**Test Coverage**:

| Test Case | What It Tests | Why It Matters |
|-----------|--------------|----------------|
| `PrintPolicyConstruction` | ActsGaudiPrintPolicy creation | Ensures policy can be instantiated correctly |
| `PrintPolicyClone` | Policy cloning mechanism | Validates cloning preserves functionality |
| `PrintPolicyFlush` | Message output functionality | Tests actual message forwarding |
| `FilterPolicyDoPrint` | Level-based filtering | Ensures messages are filtered by level |
| `FilterPolicyLevel` | Level retrieval | Tests level state management |
| `FilterPolicyClone` | Filter cloning | Validates filter duplication |
| `MakeLoggerWithMessageSvc` | Logger factory function | Tests standard logger creation |
| `MakeLoggerWithParentName` | Logger with hierarchy | Tests parent-child logger relationships |
| `ActsToGaudiLevelMapping` | Log level translation | Validates VERBOSE→FATAL level mapping |
| `LoggerUsage` | End-to-end logging | Tests actual logging workflow |
| `LoggerLevelFiltering` | Runtime filtering | Ensures level-based filtering works |

**Key Assertions**:
```cpp
// Level mapping correctness
EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::INFO));
EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::DEBUG));

// Logger creation
EXPECT_NE(logger, nullptr);
EXPECT_EQ(logger->level(), Acts::Logging::Level::INFO);
```

### 2. Helpers Tests (`test_Helpers.cpp`)

**Purpose**: Validate utility functions for coordinate transformations, unit conversions, and physics calculations.

**Test Coverage**:

| Test Category | Test Cases | Coverage |
|--------------|------------|----------|
| Unit Conversions | `ActsUnitConversions`, `DD4hepToActsUnitConversion`, `MomentumEnergyConversions` | mm↔m↔cm, GeV↔MeV, DD4hep↔Acts |
| Coordinate Systems | `BasicCoordinateTransformations`, `AngularConversions` | Cartesian↔cylindrical, deg↔rad |
| Geometry Context | `GeometryContextCreation` | Context instantiation |
| Physics Calculations | `PseudorapidityCalculations`, `ImpactParameterCalculations`, `ChiSquaredCalculations` | η, d0, χ² |
| Vector Math | `VectorMagnitudeCalculations`, `AzimuthalAngleNormalization` | Magnitude, φ normalization |
| Covariance | `CovarianceMatrixProperties` | Symmetry, positivity |

**Example Test**:
```cpp
TEST_F(HelpersTest, ActsUnitConversions) {
  double mm = 1.0 * Acts::UnitConstants::mm;
  EXPECT_DOUBLE_EQ(mm, 1.0);  // Acts uses mm as base unit
  
  double m = 1.0 * Acts::UnitConstants::m;
  EXPECT_DOUBLE_EQ(m, 1000.0);  // 1 meter = 1000 mm
}
```

### 3. Geometry Tests (`test_Geometry.cpp`)

**Purpose**: Validate geometry context operations without requiring full DD4hep setup.

**Test Coverage**:

| Test Case | Purpose |
|-----------|---------|
| `DefaultGeometryContextCreation` | Basic context creation |
| `GeometryContextCopyConstruction` | Copy semantics |
| `GeometryContextAssignment` | Assignment operator |
| `MultipleGeometryContexts` | Concurrent contexts |
| `ConstGeometryContext` | Const correctness |
| `GeometryContextLifecycle` | Object lifetime management |

**Placeholder Tests** (for future implementation):
- `SurfaceMapPlaceholder`: Surface lookup functionality
- `TrackingGeometryPlaceholder`: Full geometry hierarchy
- `AlignmentContextPlaceholder`: Alignment data handling

## Mock Objects

### MockMessageSvc

Located in `TestMocks.h`, provides a lightweight IMessageSvc implementation for testing.

**Features**:
- Captures log messages to `std::stringstream`
- Configurable output level
- Helper methods: `clearOutput()`, `getOutput()`
- Full IMessageSvc interface implementation

**Usage Example**:
```cpp
auto mockMsgSvc = std::make_shared<MockMessageSvc>();
mockMsgSvc->setOutputLevel(MSG::INFO);

// Use in tests
auto logger = makeActsGaudiLogger(mockMsgSvc.get(), "TestLogger", MSG::INFO);
ACTS_INFO(*logger, "Test message");

std::string output = mockMsgSvc->getOutput();
EXPECT_FALSE(output.empty());
```

## Writing New Tests

### Basic Test Structure

```cpp
#include <gtest/gtest.h>
#include "k4ActsTracking/YourComponent.h"

// Test fixture
class YourComponentTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Initialize test data
  }
  
  void TearDown() override {
    // Cleanup
  }
  
  // Test data members
};

// Test case
TEST_F(YourComponentTest, TestSomething) {
  // Arrange
  // Act
  // Assert
  EXPECT_TRUE(condition);
}
```

### Adding Tests to Build System

1. Create test file: `test/unit/test_YourComponent.cpp`
2. Add to `test/unit/CMakeLists.txt`:

```cmake
add_executable(test_YourComponent test_YourComponent.cpp)
target_link_libraries(test_YourComponent
  PRIVATE
  k4ActsTracking
  k4FWCore::k4FWCore
  ${ACTS_LIBRARY_TARGETS}
  GTest::gtest
  GTest::gtest_main
)
target_include_directories(test_YourComponent PRIVATE
  ${CMAKE_SOURCE_DIR}/k4ActsTracking/include
)

add_test(NAME test_YourComponent COMMAND test_YourComponent)
set_test_env(test_YourComponent)
```

### Best Practices

1. **Test Independence**: Each test should be independent and not rely on execution order
2. **Clear Names**: Use descriptive test names that explain what is being tested
3. **Arrange-Act-Assert**: Structure tests clearly
4. **One Concept Per Test**: Each test should verify one thing
5. **Use Fixtures**: Share setup/teardown code with test fixtures
6. **Mock External Dependencies**: Use mocks to isolate components
7. **Test Edge Cases**: Include tests for boundary conditions and error cases

### Assertion Macros Reference

```cpp
// Boolean assertions
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);

// Equality assertions
EXPECT_EQ(expected, actual);      // ==
EXPECT_NE(val1, val2);            // !=
EXPECT_LT(val1, val2);            // <
EXPECT_LE(val1, val2);            // <=
EXPECT_GT(val1, val2);            // >
EXPECT_GE(val1, val2);            // >=

// Floating point assertions
EXPECT_DOUBLE_EQ(expected, actual);        // Exact
EXPECT_NEAR(expected, actual, tolerance);  // Within tolerance

// Exception assertions
EXPECT_THROW(statement, exception_type);
EXPECT_NO_THROW(statement);
EXPECT_ANY_THROW(statement);

// String assertions
EXPECT_STREQ(str1, str2);
EXPECT_STRNE(str1, str2);

// Pointer assertions
EXPECT_EQ(ptr, nullptr);
EXPECT_NE(ptr, nullptr);
```

## Testing Strategies for Different Components

### Testing Services

For Gaudi services like ActsGeoSvc:
1. Mock ServiceLocator and dependent services
2. Test initialization with various configurations
3. Test public interface methods
4. Validate resource cleanup in finalize()

### Testing Algorithms

For Gaudi algorithms:
1. Mock input/output data handles
2. Test execute() with various input scenarios
3. Validate output collections
4. Test error handling and edge cases

### Testing Utilities

For helper functions:
1. Test with known input/output pairs
2. Include edge cases (zero, negative, very large values)
3. Test numerical precision
4. Validate error propagation

## Debugging Failed Tests

### Running Single Test

```bash
# Run all tests in a suite
./test/unit/test_ActsGaudiLogger

# Run specific test
./test/unit/test_ActsGaudiLogger --gtest_filter=ActsGaudiLoggerTest.PrintPolicyConstruction

# Run with verbose output
./test/unit/test_ActsGaudiLogger --gtest_filter=*FilterPolicy* --gtest_verbose
```

### Common Issues

1. **Test Fails in CI but Passes Locally**
   - Check environment differences
   - Verify dependencies are available
   - Check for hardcoded paths

2. **Floating Point Comparison Failures**
   - Use `EXPECT_NEAR()` instead of `EXPECT_EQ()`
   - Consider numerical precision limits

3. **Mock Not Capturing Output**
   - Ensure mock methods are properly overridden
   - Check that output is flushed
   - Verify mock is passed to tested component

## Test Coverage Goals

| Component | Current Coverage | Target Coverage |
|-----------|-----------------|-----------------|
| ActsGaudiLogger | ~80% (all public API) | 90% |
| Helpers/Utilities | ~60% (basic functions) | 85% |
| ActsGeoSvc | ~10% (limited by deps) | 70% |
| Algorithms | 0% (not yet in repo) | 80% |

## Future Test Development

### Priority 1: Core Components
- [ ] ActsGeoSvc initialization tests with mock DD4hep
- [ ] Surface lookup and retrieval tests
- [ ] Material decorator tests

### Priority 2: Algorithm Tests
- [ ] Track finding algorithm tests
- [ ] Seed creation tests
- [ ] Track filtering tests

### Priority 3: Integration Tests
- [ ] Full chain tests (hits → tracks)
- [ ] Multi-event processing
- [ ] Concurrent execution tests

## Resources

- [Google Test Documentation](https://google.github.io/googletest/)
- [Acts Testing Patterns](https://acts.readthedocs.io/en/latest/contributing/testing.html)
- [Gaudi Testing Guide](https://gaudi.web.cern.ch/testing/)
- [Key4hep Development Guidelines](https://key4hep.github.io/key4hep-doc/)

## Contributing

When adding new tests:
1. Follow existing patterns and naming conventions
2. Update this documentation
3. Add test descriptions to test/README.md
4. Ensure tests pass locally before committing
5. Add copyright header to new files
6. Document any new mocks or utilities
