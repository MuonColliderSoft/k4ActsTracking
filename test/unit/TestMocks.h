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

#ifndef K4ACTSTRACKING_TEST_MOCKS_H
#define K4ACTSTRACKING_TEST_MOCKS_H

#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/StatusCode.h>

#include <sstream>
#include <string>

/**
 * Mock implementations for testing k4ActsTracking components.
 * 
 * This file provides lightweight mock objects that can be used
 * in unit tests to isolate components from their dependencies.
 */

namespace k4ActsTracking {
namespace test {

/**
 * Mock implementation of IMessageSvc for testing.
 * Captures log messages for verification in tests.
 */
class MockMessageSvc : public IMessageSvc {
public:
  MockMessageSvc() = default;
  ~MockMessageSvc() override = default;

  // Captured output stream for testing
  std::stringstream capturedOutput;
  
  // Current message level
  MSG::Level currentLevel = MSG::INFO;

  // IInterface methods
  unsigned long addRef() override { return 0; }
  unsigned long release() override { return 0; }
  StatusCode queryInterface(const InterfaceID&, void**) override { 
    return StatusCode::SUCCESS; 
  }

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

  void setOutputLevel(int new_level) override { 
    currentLevel = static_cast<MSG::Level>(new_level); 
  }
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

  // Helper method to clear captured output
  void clearOutput() {
    capturedOutput.str("");
    capturedOutput.clear();
  }

  // Helper method to get captured output as string
  std::string getOutput() const {
    return capturedOutput.str();
  }
};

}  // namespace test
}  // namespace k4ActsTracking

#endif  // K4ACTSTRACKING_TEST_MOCKS_H
