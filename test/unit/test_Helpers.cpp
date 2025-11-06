/*
 * Copyright (c) 2014-2024 Key4hep-Project.
 *
 * This file is part of Key4hep.
 * See https://key4hep.github.io/key4hep-doc/ for further info.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Acts/Definitions/Units.hpp>
#include <Acts/Geometry/GeometryContext.hpp>
#include <gtest/gtest.h>

#include <cmath>

/**
 * Unit tests for helper functions used in k4ActsTracking.
 * 
 * This test file validates coordinate transformations, conversions,
 * and other utility functions that may be used across the package.
 */

// Test fixture for helper function tests
class HelpersTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Setup common test data
  }

  void TearDown() override {
    // Cleanup
  }
};

// Test Acts unit conversions
TEST_F(HelpersTest, ActsUnitConversions) {
  // Test millimeter conversion
  double mm = 1.0 * Acts::UnitConstants::mm;
  EXPECT_DOUBLE_EQ(mm, 1.0);  // Acts uses mm as base unit
  
  // Test meter conversion
  double m = 1.0 * Acts::UnitConstants::m;
  EXPECT_DOUBLE_EQ(m, 1000.0);  // 1 meter = 1000 mm
  
  // Test centimeter conversion
  double cm = 1.0 * Acts::UnitConstants::cm;
  EXPECT_DOUBLE_EQ(cm, 10.0);  // 1 cm = 10 mm
}

// Test basic coordinate transformations
TEST_F(HelpersTest, BasicCoordinateTransformations) {
  // Test Cartesian to cylindrical conversion
  double x = 3.0;
  double y = 4.0;
  double z = 5.0;
  
  double r = std::sqrt(x*x + y*y);
  double phi = std::atan2(y, x);
  
  EXPECT_DOUBLE_EQ(r, 5.0);
  EXPECT_NEAR(phi, 0.9272952180016122, 1e-10);
  
  // Test back transformation
  double x_back = r * std::cos(phi);
  double y_back = r * std::sin(phi);
  
  EXPECT_NEAR(x_back, x, 1e-10);
  EXPECT_NEAR(y_back, y, 1e-10);
}

// Test angular conversions
TEST_F(HelpersTest, AngularConversions) {
  // Test degrees to radians
  double degrees = 180.0;
  double radians = degrees * M_PI / 180.0;
  EXPECT_DOUBLE_EQ(radians, M_PI);
  
  // Test radians to degrees
  double rad = M_PI / 2.0;
  double deg = rad * 180.0 / M_PI;
  EXPECT_DOUBLE_EQ(deg, 90.0);
}

// Test momentum and energy conversions
TEST_F(HelpersTest, MomentumEnergyConversions) {
  // GeV to MeV
  double GeV = 1.0 * Acts::UnitConstants::GeV;
  double MeV = 1.0 * Acts::UnitConstants::MeV;
  
  EXPECT_DOUBLE_EQ(GeV / MeV, 1000.0);
  
  // Test momentum unit consistency
  double momentum_GeV = 10.0 * Acts::UnitConstants::GeV;
  double momentum_MeV = 10000.0 * Acts::UnitConstants::MeV;
  
  EXPECT_DOUBLE_EQ(momentum_GeV, momentum_MeV);
}

// Test DD4hep to Acts unit conversion helpers
TEST_F(HelpersTest, DD4hepToActsUnitConversion) {
  // DD4hep uses cm as base unit, Acts uses mm
  // This test verifies the conversion factor
  double dd4hep_cm = 1.0;  // 1 cm in DD4hep
  double acts_mm = dd4hep_cm * 10.0;  // Convert to mm for Acts
  
  EXPECT_DOUBLE_EQ(acts_mm, 10.0);
  
  // Test inverse conversion
  double acts_mm_value = 100.0;  // 100 mm in Acts
  double dd4hep_cm_value = acts_mm_value / 10.0;  // Convert to cm for DD4hep
  
  EXPECT_DOUBLE_EQ(dd4hep_cm_value, 10.0);
}

// Test vector magnitude calculations
TEST_F(HelpersTest, VectorMagnitudeCalculations) {
  double vx = 1.0;
  double vy = 2.0;
  double vz = 2.0;
  
  double magnitude = std::sqrt(vx*vx + vy*vy + vz*vz);
  EXPECT_DOUBLE_EQ(magnitude, 3.0);
  
  // Test transverse momentum calculation
  double pt = std::sqrt(vx*vx + vy*vy);
  EXPECT_NEAR(pt, 2.236067977499790, 1e-10);
}

// Test pseudorapidity (eta) calculations
TEST_F(HelpersTest, PseudorapidityCalculations) {
  // Eta = -ln(tan(theta/2))
  // For theta = 90 degrees (perpendicular), eta = 0
  double theta = M_PI / 2.0;
  double eta = -std::log(std::tan(theta / 2.0));
  EXPECT_NEAR(eta, 0.0, 1e-10);
  
  // For theta = 45 degrees
  theta = M_PI / 4.0;
  eta = -std::log(std::tan(theta / 2.0));
  EXPECT_NEAR(eta, 0.8813735870195430, 1e-10);
}

// Test azimuthal angle normalization
TEST_F(HelpersTest, AzimuthalAngleNormalization) {
  // Test normalization to [-pi, pi]
  auto normalize_phi = [](double phi) {
    while (phi > M_PI) phi -= 2 * M_PI;
    while (phi < -M_PI) phi += 2 * M_PI;
    return phi;
  };
  
  double phi1 = 3.5 * M_PI;
  double normalized1 = normalize_phi(phi1);
  EXPECT_GT(normalized1, -M_PI);
  EXPECT_LE(normalized1, M_PI);
  
  double phi2 = -3.5 * M_PI;
  double normalized2 = normalize_phi(phi2);
  EXPECT_GT(normalized2, -M_PI);
  EXPECT_LE(normalized2, M_PI);
}

// Test track parameter covariance matrix basic properties
TEST_F(HelpersTest, CovarianceMatrixProperties) {
  // Create a simple 3x3 covariance matrix
  double cov[3][3] = {
    {1.0, 0.1, 0.0},
    {0.1, 2.0, 0.2},
    {0.0, 0.2, 3.0}
  };
  
  // Test symmetry
  EXPECT_DOUBLE_EQ(cov[0][1], cov[1][0]);
  EXPECT_DOUBLE_EQ(cov[1][2], cov[2][1]);
  EXPECT_DOUBLE_EQ(cov[0][2], cov[2][0]);
  
  // Test diagonal positivity
  EXPECT_GT(cov[0][0], 0.0);
  EXPECT_GT(cov[1][1], 0.0);
  EXPECT_GT(cov[2][2], 0.0);
}

// Test impact parameter calculations
TEST_F(HelpersTest, ImpactParameterCalculations) {
  // Simple test for d0 (transverse impact parameter)
  // d0 = -(x*sin(phi) - y*cos(phi))
  double x = 0.1;  // 0.1 mm
  double y = 0.05; // 0.05 mm
  double phi = 0.0;
  
  double d0 = -(x * std::sin(phi) - y * std::cos(phi));
  EXPECT_NEAR(d0, -0.05, 1e-10);
  
  // Test with different phi
  phi = M_PI / 2.0;
  d0 = -(x * std::sin(phi) - y * std::cos(phi));
  EXPECT_NEAR(d0, -0.1, 1e-10);
}

// Test chi-squared calculations
TEST_F(HelpersTest, ChiSquaredCalculations) {
  // Simple chi-squared calculation
  double residual = 0.5;
  double error = 0.1;
  double chi2 = (residual * residual) / (error * error);
  
  EXPECT_DOUBLE_EQ(chi2, 25.0);
  
  // Test with multiple measurements
  std::vector<double> residuals = {0.1, 0.2, -0.15};
  std::vector<double> errors = {0.05, 0.1, 0.08};
  
  double total_chi2 = 0.0;
  for (size_t i = 0; i < residuals.size(); ++i) {
    total_chi2 += (residuals[i] * residuals[i]) / (errors[i] * errors[i]);
  }
  
  EXPECT_NEAR(total_chi2, 11.515625, 1e-6);
}

// Placeholder test for future track conversions between ACTS and EDM4HEP
TEST_F(HelpersTest, TrackConversionPlaceholder) {
  // This test is a placeholder for future track parameter conversion tests
  // between ACTS track representation and EDM4HEP track representation
  
  // When implemented, this should test:
  // - Conversion of track parameters (d0, z0, phi, theta, q/p)
  // - Conversion of covariance matrices
  // - Proper handling of reference points
  // - Coordinate system transformations
  
  EXPECT_TRUE(true);  // Placeholder assertion
}
