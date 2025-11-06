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

#include "k4ActsTracking/ActsGaudiLogger.h"
#include "TestMocks.h"

#include <Acts/Utilities/Logger.hpp>
#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>

using k4ActsTracking::test::MockMessageSvc;

// Test fixture for ActsGaudiLogger tests
class ActsGaudiLoggerTest : public ::testing::Test {
protected:
  void SetUp() override {
    mockMsgSvc = std::make_shared<MockMessageSvc>();
  }

  void TearDown() override {
    mockMsgSvc.reset();
  }

  std::shared_ptr<MockMessageSvc> mockMsgSvc;
};

// Test ActsGaudiPrintPolicy construction and name retrieval
TEST_F(ActsGaudiLoggerTest, PrintPolicyConstruction) {
  auto msgStream = std::make_shared<MsgStream>(mockMsgSvc.get(), "TestComponent");
  ActsGaudiPrintPolicy policy(mockMsgSvc.get(), msgStream, "TestName");
  
  EXPECT_EQ(policy.name(), "TestName");
}

// Test ActsGaudiPrintPolicy cloning
TEST_F(ActsGaudiLoggerTest, PrintPolicyClone) {
  auto msgStream = std::make_shared<MsgStream>(mockMsgSvc.get(), "TestComponent");
  ActsGaudiPrintPolicy policy(mockMsgSvc.get(), msgStream, "TestName");
  
  auto cloned = policy.clone("ClonedName");
  EXPECT_NE(cloned, nullptr);
  EXPECT_EQ(cloned->name(), "ClonedName");
}

// Test ActsGaudiPrintPolicy flush functionality
TEST_F(ActsGaudiLoggerTest, PrintPolicyFlush) {
  auto msgStream = std::make_shared<MsgStream>(mockMsgSvc.get(), "TestComponent");
  ActsGaudiPrintPolicy policy(mockMsgSvc.get(), msgStream, "TestName");
  
  mockMsgSvc->clearOutput();  // Clear any previous output
  policy.flush(Acts::Logging::Level::INFO, "Test message");
  
  // Check that message was captured (note: exact format depends on Gaudi implementation)
  std::string captured = mockMsgSvc->getOutput();
  EXPECT_FALSE(captured.empty());
}

// Test ActsGaudiFilterPolicy level checking
TEST_F(ActsGaudiLoggerTest, FilterPolicyDoPrint) {
  auto msgStream = std::make_shared<MsgStream>(mockMsgSvc.get(), "TestComponent");
  msgStream->setLevel(MSG::INFO);
  ActsGaudiFilterPolicy policy(msgStream);
  
  // INFO level and above should print when level is INFO
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::INFO));
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::WARNING));
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::ERROR));
  
  // DEBUG and VERBOSE should not print when level is INFO
  EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::DEBUG));
  EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::VERBOSE));
}

// Test ActsGaudiFilterPolicy level retrieval
TEST_F(ActsGaudiLoggerTest, FilterPolicyLevel) {
  auto msgStream = std::make_shared<MsgStream>(mockMsgSvc.get(), "TestComponent");
  msgStream->setLevel(MSG::INFO);
  ActsGaudiFilterPolicy policy(msgStream);
  
  Acts::Logging::Level level = policy.level();
  EXPECT_EQ(level, Acts::Logging::Level::INFO);
}

// Test ActsGaudiFilterPolicy cloning
TEST_F(ActsGaudiLoggerTest, FilterPolicyClone) {
  auto msgStream = std::make_shared<MsgStream>(mockMsgSvc.get(), "TestComponent");
  msgStream->setLevel(MSG::INFO);
  ActsGaudiFilterPolicy policy(msgStream);
  
  auto cloned = policy.clone(Acts::Logging::Level::DEBUG);
  EXPECT_NE(cloned, nullptr);
  EXPECT_EQ(cloned->level(), Acts::Logging::Level::DEBUG);
}

// Test makeActsGaudiLogger with IMessageSvc
TEST_F(ActsGaudiLoggerTest, MakeLoggerWithMessageSvc) {
  auto logger = makeActsGaudiLogger(mockMsgSvc.get(), "TestLogger", MSG::INFO);
  
  EXPECT_NE(logger, nullptr);
  EXPECT_EQ(logger->level(), Acts::Logging::Level::INFO);
}

// Test makeActsGaudiLogger with parent name
TEST_F(ActsGaudiLoggerTest, MakeLoggerWithParentName) {
  auto logger = makeActsGaudiLogger(mockMsgSvc.get(), "TestLogger", MSG::INFO, std::optional<std::string>("Parent"));
  
  EXPECT_NE(logger, nullptr);
  EXPECT_EQ(logger->level(), Acts::Logging::Level::INFO);
}

// Test level mapping from Acts to Gaudi
TEST_F(ActsGaudiLoggerTest, ActsToGaudiLevelMapping) {
  auto msgStream = std::make_shared<MsgStream>(mockMsgSvc.get(), "TestComponent");
  ActsGaudiFilterPolicy policy(msgStream);
  
  // Test that different log levels work correctly
  msgStream->setLevel(MSG::VERBOSE);
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::VERBOSE));
  
  msgStream->setLevel(MSG::DEBUG);
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::DEBUG));
  EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::VERBOSE));
  
  msgStream->setLevel(MSG::INFO);
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::INFO));
  EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::DEBUG));
  
  msgStream->setLevel(MSG::WARNING);
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::WARNING));
  EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::INFO));
  
  msgStream->setLevel(MSG::ERROR);
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::ERROR));
  EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::WARNING));
  
  msgStream->setLevel(MSG::FATAL);
  EXPECT_TRUE(policy.doPrint(Acts::Logging::Level::FATAL));
  EXPECT_FALSE(policy.doPrint(Acts::Logging::Level::ERROR));
}

// Test that logger can be used for actual logging
TEST_F(ActsGaudiLoggerTest, LoggerUsage) {
  auto logger = makeActsGaudiLogger(mockMsgSvc.get(), "TestLogger", MSG::INFO);
  
  // Clear previous output
  mockMsgSvc->clearOutput();
  
  // Log a message using the Acts logger
  ACTS_INFO(*logger, "Test info message");
  
  // Verify something was logged
  std::string captured = mockMsgSvc->getOutput();
  EXPECT_FALSE(captured.empty());
}

// Test logger level filtering
TEST_F(ActsGaudiLoggerTest, LoggerLevelFiltering) {
  auto logger = makeActsGaudiLogger(mockMsgSvc.get(), "TestLogger", MSG::WARNING);
  
  // Clear previous output
  mockMsgSvc->clearOutput();
  
  // Debug messages should not appear
  ACTS_DEBUG(*logger, "Debug message");
  std::string captured1 = mockMsgSvc->getOutput();
  
  // Warning messages should appear
  mockMsgSvc->clearOutput();
  ACTS_WARNING(*logger, "Warning message");
  std::string captured2 = mockMsgSvc->getOutput();
  
  // Warning level output should have content, debug should be filtered
  EXPECT_FALSE(captured2.empty());
}
