#include "gtest/gtest.h"
#include "list-ptr.h"

#include <cstdint>
#include <cstdlib>
#include <new>

static thread_local std::size_t allocations = 0;

void* operator new(std::size_t count) {
  ++allocations;
  void* ptr = std::malloc(count);
  if (!ptr) {
    throw std::bad_alloc();
  }
  return ptr;
}

void* operator new[](std::size_t count) {
  return operator new(count);
}

void* operator new(std::size_t count, const std::nothrow_t&) noexcept {
  ++allocations;
  void* ptr = std::malloc(count);
  return ptr;
}

void* operator new[](std::size_t count, const std::nothrow_t& token) noexcept {
  return operator new(count, token);
}

void operator delete(void* ptr) noexcept {
  std::free(ptr);
}

void operator delete[](void* ptr) noexcept {
  operator delete(ptr);
}

void operator delete(void* ptr, std::size_t) noexcept {
  operator delete(ptr);
}

void operator delete[](void* ptr, std::size_t) noexcept {
  operator delete(ptr);
}

namespace ct::test {

class A {
public:
  A(int) {}

  A() {}

  A(const A&) {}

  A(A&&) {}

  A& operator=(const A&) {
    return *this;
  }

  A& operator=(A&&) {
    return *this;
  }
};

class B : public A {
public:
  B(int v)
      : A(v) {}
};

class AllocationTest : public ::testing::Test {
public:
  A* data;
  A* more_data;
  B* another_data;

protected:
  void SetUp() override {
    data = new A(42);
    more_data = new A(43);
    another_data = new B(44);
    allocations = 0;
  }

  virtual void CleanUp() {}
};

class NoAllocExpected : public AllocationTest {
protected:
  void TearDown() override {
    EXPECT_EQ(allocations, 0);
  }
};

class AllocOnceExpected : public AllocationTest {
protected:
  void TearDown() override {
    EXPECT_EQ(allocations, 1);
  }
};

using Ptr = ListPtr<A>;

TEST_F(NoAllocExpected, DefaultCtor) {
  Ptr p;
}

TEST_F(NoAllocExpected, NullptrCtor) {
  Ptr p(nullptr);
}

TEST_F(NoAllocExpected, PtrCtor) {
  Ptr p(data);
}

TEST_F(NoAllocExpected, PtrDeleterCtor) {
  auto deleter = [](auto* ptr) {
    delete ptr;
  };
  ListPtr<A, void (*)(A*)> p(data, deleter);
}

TEST_F(NoAllocExpected, CopyCtor) {
  Ptr p(data);
  Ptr q = p;
}

TEST_F(NoAllocExpected, MoveCtor) {
  Ptr p(data);
  Ptr q = std::move(p);
}

TEST_F(NoAllocExpected, ConvertibleLvalueCtor) {
  ListPtr<B> p(another_data);
  ListPtr<A> q = p;
}

TEST_F(NoAllocExpected, ConvertibleXvalueCtor) {
  ListPtr<B> p(another_data);
  ListPtr<A> q = std::move(p);
}

TEST_F(NoAllocExpected, ConvertiblePrvalueCtor) {
  ListPtr<A> q = ListPtr<B>(another_data);
}

TEST_F(NoAllocExpected, CopyAssign) {
  Ptr p(data);
  Ptr q;
  q = p;
}

TEST_F(NoAllocExpected, MoveAssign) {
  Ptr p(data);
  Ptr q;
  q = std::move(p);
}

TEST_F(NoAllocExpected, LvalueConvAssign) {
  ListPtr<B> p(another_data);
  ListPtr<A> q(data);
  q = p;
}

TEST_F(NoAllocExpected, XvalueConvAssign) {
  ListPtr<A> p(data);
  ListPtr<B> q(another_data);
  p = std::move(q);
}

TEST_F(NoAllocExpected, PrvalueConvAssign) {
  ListPtr<A> q(data);
  q = Ptr(data);
}

TEST_F(NoAllocExpected, ConstOperations) {
  Ptr p(data);
  EXPECT_EQ(p.get(), data);
  EXPECT_TRUE(static_cast<bool>(p));
  EXPECT_EQ(p.operator->(), data);
  EXPECT_EQ(p.useCount(), 1);
}

TEST_F(NoAllocExpected, Reset) {
  Ptr p(data);
  p.reset();

  Ptr q(data);
  q.reset(more_data);
}

TEST_F(NoAllocExpected, Release) {
  Ptr p(data);
  p.release();
}

TEST_F(NoAllocExpected, Comparisons) {
  Ptr p(data);
  Ptr q = p;
  EXPECT_EQ(p, q);
  Ptr r(more_data);
  EXPECT_NE(p, r);
}

TEST_F(AllocOnceExpected, Make) {
  auto p = makeListPtr<int>(42);
}

} // namespace ct::test
