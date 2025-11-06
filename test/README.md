# k4ActsTracking Test Suite

This directory contains the comprehensive test suite for the k4ActsTracking package, designed to ensure robustness and maintainability of the tracking software.

## Test Structure

### Unit Tests (`unit/`)

Unit tests verify individual components and functions in isolation. These tests use Google Test framework.

#### Current Unit Tests

1. **test_ActsGaudiLogger.cpp**
   - Tests for ActsGaudiLogger integration
   - Verifies ACTS ↔ Gaudi log level mapping
   - Tests ActsGaudiPrintPolicy functionality
   - Tests ActsGaudiFilterPolicy functionality
   - Validates logger creation and usage
   - Coverage:
     - Level mapping (MSG::Level ↔ Acts::Logging::Level)
     - Print policy construction, cloning, and flush
     - Filter policy level checking and filtering
     - Logger creation with various parameters
     - Actual logging and level-based filtering

2. **test_Helpers.cpp**
   - Tests for helper functions and coordinate transformations
   - Validates unit conversions (Acts, DD4hep)
   - Tests geometric calculations
   - Coverage:
     - Acts unit conversions (mm, cm, m, GeV, MeV)
     - DD4hep to Acts unit conversion
     - Coordinate transformations (Cartesian ↔ cylindrical)
     - Angular conversions (degrees ↔ radians)
     - Pseudorapidity calculations
     - Impact parameter calculations
     - Chi-squared calculations
     - Covariance matrix property validation
     - Placeholder for future ACTS ↔ EDM4HEP conversions

### Integration Tests (`options/`)

Integration tests verify the complete workflow and component interactions using the Gaudi framework via k4run.

#### Current Integration Tests

1. **LoadODD**
   - Tests initialization of ActsGeoSvc with DD4hep
   - Loads Open Data Detector (ODD) geometry
   - Verifies geometry conversion from DD4hep to ACTS
   - Tests geometry visualization (OBJ file creation)

2. **CheckODDObjFile**
   - Validates that geometry OBJ file was created successfully
   - Depends on LoadODD test passing

## Running Tests

### Prerequisites

- Key4hep software stack installed
- Google Test library (for unit tests)
- Access to Open Data Detector geometry files

### Building with Tests

```bash
mkdir build install
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install -DBUILD_TESTING=ON
make
```

### Running All Tests

```bash
cd build
ctest
```

### Running Specific Tests

```bash
# Run only unit tests
ctest -R test_

# Run specific unit test
./test/unit/test_ActsGaudiLogger
./test/unit/test_Helpers

# Run only integration tests
ctest -R LoadODD
```

### Verbose Test Output

```bash
ctest --verbose
# or
ctest --output-on-failure
```

## Test Development Guidelines

### Writing Unit Tests

1. Use Google Test framework (gtest)
2. Follow existing test file structure
3. Use descriptive test names: `TEST_F(FixtureName, TestName)`
4. Group related tests using test fixtures
5. Keep tests independent and isolated
6. Mock external dependencies when appropriate
7. Add copyright header to all new test files

### Test Naming Conventions

- Test files: `test_<ComponentName>.cpp`
- Test fixtures: `<ComponentName>Test`
- Test cases: Descriptive names indicating what is being tested

### Adding New Unit Tests

1. Create test file in `test/unit/`
2. Add executable and test registration in `test/unit/CMakeLists.txt`
3. Follow existing test patterns
4. Document test coverage in this README

### Adding New Integration Tests

1. Create Python configuration in `test/options/`
2. Add test in `test/CMakeLists.txt` using `add_test()`
3. Set up test environment using `set_test_env()`
4. Document test purpose in this README

## Future Test Development

Based on the test suite proposal, the following tests are planned:

### Unit Tests (To Be Implemented)

- **ActsGeoSvc Tests**
  - Tracking geometry construction validation
  - Surface lookup and retrieval
  - Material decorator application
  - Mock-based testing without full DD4hep

### Algorithm Component Tests (To Be Implemented)

When tracking algorithms are added to the package:

- **ACTSSeededCKFTrackingAlg Tests**
  - Configuration and initialization
  - Seed creation from tracker hits
  - CKF track finding
  - Output validation
  - Edge cases handling

- **ACTSDuplicateRemoval Tests**
  - Duplicate identification logic
  - Track quality preservation
  - Edge cases

- **FilterTracksAlg Tests**
  - Filter cut handling
  - Physics cut validation

- **TrackTruthAlg Tests**
  - Track-MC association
  - Simulated hit relations

### Integration Tests (To Be Implemented)

- **Full Tracking Chain**
  - Hits → Seeds → Tracks workflow
  - Realistic detector scenarios
  - Efficiency and correctness validation

- **Service Integration**
  - Multiple detector configurations
  - Magnetic field variations
  - Concurrent service usage

### Validation Tests (To Be Implemented)

- **Physics Validation**
  - Track parameter resolution
  - Momentum reconstruction accuracy
  - Impact parameter accuracy
  - Track finding efficiency metrics
  - Fake rate measurements

### Regression Tests (To Be Implemented)

- Reference output storage
- Automated comparison with baselines
- Performance regression detection

## Continuous Integration

Tests are automatically run in CI/CD pipelines:

- **Per Pull Request**: Unit tests and basic integration tests
- **Nightly/Weekly**: Full integration and physics validation tests
- **Artifacts**: Coverage reports and performance metrics

## Test Data

Test data requirements:

- Open Data Detector (ODD) geometry (provided by OpenDataDetector package)
- Sample tracker hits (to be added)
- MC truth data (to be added)
- Reference outputs for regression tests (to be added)

## Contributing

When contributing new code:

1. Add corresponding unit tests
2. Ensure all tests pass locally
3. Add integration tests for new workflows
4. Update this README with new test descriptions
5. Follow the existing test patterns and conventions

## References

- [Acts Documentation](https://acts.readthedocs.io/)
- [Gaudi Framework](https://gaudi.web.cern.ch/)
- [Google Test Documentation](https://google.github.io/googletest/)
- [Key4hep Documentation](https://key4hep.github.io/key4hep-doc/)
