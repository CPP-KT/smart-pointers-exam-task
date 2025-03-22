#include "test-object.h"

#include <gtest/gtest.h>

namespace {

int transcode(int data, const void* ptr) {
  return data ^ static_cast<int>(reinterpret_cast<std::ptrdiff_t>(ptr) / sizeof(ct::test::TestObject));
}

} // namespace

namespace ct::test {

TestObject::TestObject(int data)
    : data(transcode(data, this)) {
  auto p = instances.insert(this);
  EXPECT_TRUE(p.second);
}

TestObject::TestObject(const TestObject& other) {
  {
    EXPECT_TRUE(instances.find(&other) != instances.end());
    auto p = instances.insert(this);
    EXPECT_TRUE(p.second);
  }
  data = transcode(transcode(other.data, &other), this);
}

TestObject::~TestObject() {
  size_t n = instances.erase(this);
  EXPECT_EQ(1u, n);
}

TestObject& TestObject::operator=(const TestObject& c) {
  EXPECT_TRUE(instances.find(this) != instances.end());
  data = transcode(transcode(c.data, &c), this);
  return *this;
}

TestObject::operator int() const {
  EXPECT_TRUE(instances.find(this) != instances.end());

  return transcode(data, this);
}

std::set<const TestObject*> TestObject::instances;

TestObject::NoNewInstancesGuard::NoNewInstancesGuard()
    : old_instances(instances) {}

TestObject::NoNewInstancesGuard::~NoNewInstancesGuard() {
  EXPECT_TRUE(old_instances == instances);
}

void TestObject::NoNewInstancesGuard::expect_no_instances() const {
  EXPECT_TRUE(old_instances == instances);
}

} // namespace ct::test
