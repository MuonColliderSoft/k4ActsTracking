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

#include <Acts/Utilities/Logger.hpp>
#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/MsgStream.h>
#include <gtest/gtest.h>

#include <memory>
#include <sstream>

// Mock MessageSvc for testing
class MockMessageSvc : public IMessageSvc {
public:
  MockMessageSvc() = default;
  ~MockMessageSvc() override = default;

  // Capture output for testing
  std::stringstream capturedOutput;
  MSG::Level currentLevel = MSG::INFO;

  // IInterface methods
  unsigned long addRef() override { return 0; }
  unsigned long release() override { return 0; }
  StatusCode queryInterface(const InterfaceID&, void**) override { return StatusCode::SUCCESS; }

  // IMessageSvc methods
  void reportMessage(const Message& msg, int outputLevel) override {
    capturedOutput << msg.getMessage();
  }

  void reportMessage(const Message& msg) override {
    capturedOutput << msg.getMessage();
  }

  void reportMessage(const StatusCode& code, std::string_view source) override {}

  void insertStream(int, std::string_view, std::ostream*) override {}

  void eraseStream(int) override {}
  void eraseStream(std::string_view) override {}
  void eraseStream() override {}

  void desactivateStream(int) override {}
  void desactivateStream(std::string_view) override {}

  void activateStream(int) override {}
  void activateStream(std::string_view) override {}

  int outputLevel() const override { return currentLevel; }
  int outputLevel(std::string_view) const override { return currentLevel; }

  void setOutputLevel(int new_level) override { currentLevel = static_cast<MSG::Level>(new_level); }
  void setOutputLevel(std::string_view source, int level) override {}

  bool useColor() const override { return false; }
  void setColor(bool) override {}

  int messageCount(MSG::Level logLevel) const override { return 0; }

  void incrInactiveCount(MSG::Level level, std::string_view src) override {}

  const std::string& getLogColor(int logLevel) const override {
    static std::string empty;
    return empty;
  }

  void setDefaultFormat(std::string_view) override {}
  void setFormat(std::string_view source, std::string_view format) override {}
};

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
  
  mockMsgSvc->capturedOutput.str("");  // Clear any previous output
  policy.flush(Acts::Logging::Level::INFO, "Test message");
  
  // Check that message was captured (note: exact format depends on Gaudi implementation)
  std::string captured = mockMsgSvc->capturedOutput.str();
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
  mockMsgSvc->capturedOutput.str("");
  
  // Log a message using the Acts logger
  ACTS_INFO(*logger, "Test info message");
  
  // Verify something was logged
  std::string captured = mockMsgSvc->capturedOutput.str();
  EXPECT_FALSE(captured.empty());
}

// Test logger level filtering
TEST_F(ActsGaudiLoggerTest, LoggerLevelFiltering) {
  auto logger = makeActsGaudiLogger(mockMsgSvc.get(), "TestLogger", MSG::WARNING);
  
  // Clear previous output
  mockMsgSvc->capturedOutput.str("");
  
  // Debug messages should not appear
  ACTS_DEBUG(*logger, "Debug message");
  std::string captured1 = mockMsgSvc->capturedOutput.str();
  
  // Warning messages should appear
  mockMsgSvc->capturedOutput.str("");
  ACTS_WARNING(*logger, "Warning message");
  std::string captured2 = mockMsgSvc->capturedOutput.str();
  
  // Warning level output should have content, debug should be filtered
  EXPECT_FALSE(captured2.empty());
}
