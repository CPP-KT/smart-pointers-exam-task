#pragma once

#include <set>

namespace ct::test {

struct TestObject {
  struct NoNewInstancesGuard;

  TestObject() = delete;
  explicit TestObject(int data);
  TestObject(const TestObject& other);
  ~TestObject();

  TestObject& operator=(const TestObject& c);
  operator int() const;

private:
  int data;

  static std::set<const TestObject*> instances;
};

struct TestObject::NoNewInstancesGuard {
  NoNewInstancesGuard();

  NoNewInstancesGuard(const NoNewInstancesGuard&) = delete;
  NoNewInstancesGuard& operator=(const NoNewInstancesGuard&) = delete;

  ~NoNewInstancesGuard();

  void expect_no_instances() const;

private:
  std::set<const TestObject*> old_instances;
};

} // namespace ct::test
