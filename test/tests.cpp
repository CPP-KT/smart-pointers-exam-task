#include "list-ptr.h"
#include "test-classes.h"
#include "test-object.h"

#include <gtest/gtest.h>

namespace ct::test {

class ListPtrTest : public ::testing::Test {
protected:
  TestObject::NoNewInstancesGuard instances_guard;
};

using Ptr = ListPtr<TestObject>;

inline static constexpr int magic = 42;

TEST_F(ListPtrTest, DefaultCtor) {
  Ptr p;
  EXPECT_EQ(nullptr, p.get());
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, PtrCtor) {
  auto* p = new TestObject(magic);
  Ptr q(p);
  EXPECT_TRUE(static_cast<bool>(q));
  EXPECT_EQ(p, q.get());
  EXPECT_EQ(magic, *q);
}

TEST_F(ListPtrTest, PtrCtorNullptr) {
  Ptr p(nullptr);
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_EQ(0, p.useCount());
}

TEST_F(ListPtrTest, PtrCtorNonEmptyNullptr) {
  Ptr p(static_cast<TestObject*>(nullptr));
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_EQ(1, p.useCount());
}

TEST_F(ListPtrTest, ConstDereferencing) {
  const Ptr p(new TestObject(magic));
  EXPECT_EQ(42, *p);
  EXPECT_EQ(42, p->operator int());
}

TEST_F(ListPtrTest, Reset) {
  Ptr q(new TestObject(magic));
  EXPECT_TRUE(static_cast<bool>(q));
  q.reset();
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, ResetNullptr) {
  Ptr q;
  EXPECT_FALSE(static_cast<bool>(q));
  q.reset();
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, ResetPtr) {
  Ptr q(new TestObject(magic));
  EXPECT_TRUE(static_cast<bool>(q));
  q.reset(new TestObject(magic + 1));
  EXPECT_EQ(magic + 1, *q);
}

TEST_F(ListPtrTest, CopyCtor) {
  Ptr p(new TestObject(magic));
  EXPECT_EQ(1, p.useCount());
  Ptr q = p;
  EXPECT_TRUE(static_cast<bool>(p));
  EXPECT_TRUE(static_cast<bool>(q));
  EXPECT_TRUE(p == q);
  EXPECT_EQ(42, *p);
  EXPECT_EQ(42, *q);
  EXPECT_EQ(2, q.useCount());
}

TEST_F(ListPtrTest, CopyCtorNullptr) {
  Ptr p;
  Ptr q = p;
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, MoveCtor) {
  Ptr p(new TestObject(magic));
  Ptr q = std::move(p);
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_TRUE(static_cast<bool>(q));
  EXPECT_EQ(magic, *q);
}

TEST_F(ListPtrTest, MoveCtorNullptr) {
  Ptr p;
  Ptr q = std::move(p);
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, CopyAssignmentOperator) {
  Ptr p(new TestObject(magic));
  Ptr q(new TestObject(magic + 1));
  p = q;
  EXPECT_EQ(magic + 1, *p);
  EXPECT_TRUE(p == q);
}

TEST_F(ListPtrTest, CopyAssignmentOperatorFromNullptr) {
  Ptr p(new TestObject(magic));
  Ptr q;
  p = q;
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, CopyAssignmentOperatorToNullptr) {
  Ptr p;
  Ptr q(new TestObject(magic));
  p = q;
  EXPECT_EQ(magic, *p);
  EXPECT_TRUE(p == q);
}

TEST_F(ListPtrTest, CopyAssignmentOperatorNullptr) {
  Ptr p;
  Ptr q;
  p = q;
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, CopyAssignmentOperatorSelf) {
  Ptr p(new TestObject(magic));
  p = p;
  EXPECT_EQ(magic, *p);
}

TEST_F(ListPtrTest, CopyAssignmentOperatorSelfNullptr) {
  Ptr p;
  p = p;
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, MoveAssignmentOperator) {
  Ptr p(new TestObject(magic));
  Ptr q(new TestObject(magic + 1));
  p = std::move(q);
  EXPECT_EQ(magic + 1, *p);
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, MoveAssignmentOperatorFromNullptr) {
  Ptr p(new TestObject(magic));
  Ptr q;
  p = std::move(q);
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, MoveAssignmentOperatorToNullptr) {
  Ptr p;
  Ptr q(new TestObject(magic + 1));
  p = std::move(q);
  EXPECT_EQ(magic + 1, *p);
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, MoveAssignmentOperatorNullptr) {
  Ptr p;
  Ptr q;
  p = std::move(q);
  EXPECT_FALSE(static_cast<bool>(p));
  EXPECT_FALSE(static_cast<bool>(q));
}

TEST_F(ListPtrTest, MoveAssignmentOperatorSelf) {
  Ptr p(new TestObject(magic));
  p = std::move(p);
  EXPECT_EQ(magic, *p);
}

TEST_F(ListPtrTest, MoveAssignmentOperatorSelfNullptr) {
  Ptr p;
  p = std::move(p);
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, MakeShared) {
  Ptr p = makeListPtr<TestObject>(magic);
  EXPECT_EQ(magic, *p);
}

TEST_F(ListPtrTest, MakeSharedForwarding) {
  struct Pair {
    Pair(int& x, double&& y)
        : x(x)
        , y(y) {}

    int& x;
    double y;
  };

  int x = magic;
  ListPtr<Pair> p = makeListPtr<Pair>(x, 3.14);
  EXPECT_EQ(p->x, magic);
  EXPECT_EQ(&p->x, &x);
  EXPECT_EQ(p->y, 3.14);
}

TEST_F(ListPtrTest, PtrCtorInheritance) {
  bool deleted = false;
  { ListPtr<DestructionTrackerBase> p(new DestructionTracker(&deleted)); }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, ResetPtrInheritance) {
  bool deleted = false;
  {
    ListPtr<DestructionTrackerBase> p;
    p.reset(new DestructionTracker(&deleted));
  }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, CustomDeleter) {
  bool deleted = false;
  { ListPtr<TestObject, TrackingDeleter<TestObject>> p(new TestObject(magic), TrackingDeleter<TestObject>(&deleted)); }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, CustomDeleterFinal) {
  struct Deleter final : TrackingDeleter<TestObject> {
    using TrackingDeleter::TrackingDeleter;
  };

  bool deleted = false;
  { ListPtr<TestObject, Deleter> p(new TestObject(magic), Deleter(&deleted)); }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, CustomDeleterFunctionPointer) {
  thread_local bool deleted;
  deleted = false;
  {
    auto deleter = [](TestObject* ptr) {
      deleted = true;
      delete ptr;
    };
    ListPtr<TestObject, void (*)(TestObject*)> p(new TestObject(magic), static_cast<void (*)(TestObject*)>(deleter));
  }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, CustomDeleterLvalue) {
  thread_local bool deleted;
  deleted = false;
  {
    auto deleter = [](TestObject* ptr) {
      deleted = true;
      delete ptr;
    };
    ListPtr<TestObject, void (*)(TestObject*)> p(new TestObject(magic), deleter);
  }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, InheritanceConvertible) {
  using Base = ListPtr<DestructionTrackerBase>;
  using ConstBase = ListPtr<const DestructionTrackerBase>;
  using Derived = ListPtr<DestructionTracker>;
  using ConstDerived = ListPtr<const DestructionTracker>;

  EXPECT_TRUE((std::is_convertible_v<const Derived&, Base>) );
  EXPECT_TRUE(!(std::is_convertible_v<const Base&, Derived>) );

  EXPECT_TRUE((std::is_convertible_v<const ConstDerived&, ConstBase>) );
  EXPECT_TRUE(!(std::is_convertible_v<const ConstBase&, ConstDerived>) );

  EXPECT_TRUE((std::is_convertible_v<const Derived&, ConstBase>) );
  EXPECT_TRUE(!(std::is_convertible_v<const Base&, ConstDerived>) );

  EXPECT_TRUE(!(std::is_convertible_v<const ConstDerived&, Base>) );
  EXPECT_TRUE(!(std::is_convertible_v<const ConstBase&, Derived>) );
}

TEST_F(ListPtrTest, CopyCtorConst) {
  Ptr p(new TestObject(magic));
  ListPtr<const TestObject> q = p;
  EXPECT_EQ(magic, *q);
}

TEST_F(ListPtrTest, MoveCtorConst) {
  Ptr p(new TestObject(magic));
  ListPtr<const TestObject> q = std::move(p);
  EXPECT_EQ(magic, *q);
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, CopyAssignmentOperatorConst) {
  Ptr p(new TestObject(magic));
  ListPtr<const TestObject> q(new TestObject(magic + 1));
  q = p;
  EXPECT_EQ(magic, *q);
  EXPECT_EQ(magic, *p);
}

TEST_F(ListPtrTest, CopyAssignmentOperatorConstToNullptr) {
  Ptr p(new TestObject(magic));
  ListPtr<const TestObject> q;
  q = p;
  EXPECT_EQ(magic, *q);
  EXPECT_EQ(magic, *p);
}

TEST_F(ListPtrTest, CopyAssignmentOperatorConstFromNullptr) {
  Ptr p;
  ListPtr<const TestObject> q(new TestObject(magic + 1));
  q = p;
  EXPECT_FALSE(static_cast<bool>(q));
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, MoveAssignmentOperatorConst) {
  Ptr p(new TestObject(magic));
  ListPtr<const TestObject> q(new TestObject(magic + 1));
  q = std::move(p);
  EXPECT_EQ(magic, *q);
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, MoveAssignmentOperatorConstToNullptr) {
  Ptr p(new TestObject(magic));
  ListPtr<const TestObject> q;
  q = std::move(p);
  EXPECT_EQ(magic, *q);
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, MoveAssignmentOperatorConstFromNullptr) {
  Ptr p;
  ListPtr<const TestObject> q(new TestObject(magic + 1));
  q = std::move(p);
  EXPECT_FALSE(static_cast<bool>(q));
  EXPECT_FALSE(static_cast<bool>(p));
}

TEST_F(ListPtrTest, CopyCtorInheritance) {
  bool deleted = false;
  {
    DestructionTracker* ptr = new DestructionTracker(&deleted);
    ListPtr<DestructionTracker> d(ptr);
    {
      ListPtr<DestructionTrackerBase> b = d;
      EXPECT_EQ(ptr, b.get());
      EXPECT_EQ(ptr, d.get());
    }
    EXPECT_FALSE(deleted);
  }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, MoveCtorInheritance) {
  bool deleted = false;
  {
    DestructionTracker* ptr = new DestructionTracker(&deleted);
    ListPtr<DestructionTracker> d(ptr);
    {
      ListPtr<DestructionTrackerBase> b = std::move(d);
      EXPECT_EQ(ptr, b.get());
      EXPECT_FALSE(static_cast<bool>(d));
    }
    EXPECT_TRUE(deleted);
    deleted = false;
  }
  EXPECT_FALSE(deleted);
}

TEST_F(ListPtrTest, CopyAssignmentOperatorInheritance) {
  bool deleted = false;
  {
    auto* ptr = new DestructionTracker(&deleted);
    ListPtr<DestructionTracker> d(ptr);
    {
      ListPtr<DestructionTrackerBase> b(new DestructionTrackerBase());
      b = d;
      EXPECT_EQ(ptr, b.get());
      EXPECT_EQ(ptr, d.get());
    }
    EXPECT_FALSE(deleted);
  }
  EXPECT_TRUE(deleted);
}

TEST_F(ListPtrTest, MoveAssignmentOperatorInheritance) {
  bool deleted = false;
  {
    auto* ptr = new DestructionTracker(&deleted);
    ListPtr<DestructionTracker> d(ptr);
    {
      ListPtr<DestructionTrackerBase> b(new DestructionTrackerBase());
      b = std::move(d);
      EXPECT_EQ(ptr, b.get());
      EXPECT_FALSE(static_cast<bool>(d));
    }
    EXPECT_TRUE(deleted);
    deleted = false;
  }
  EXPECT_FALSE(deleted);
}

TEST_F(ListPtrTest, Equivalence) {
  Ptr p1(new TestObject(magic));
  Ptr p2(new TestObject(magic + 1));

  EXPECT_FALSE(p1 == p2);
  EXPECT_TRUE(p1 != p2);
}

TEST_F(ListPtrTest, EquivalenceSelf) {
  Ptr p(new TestObject(magic));

  EXPECT_TRUE(p == p);
  EXPECT_FALSE(p != p);
}

TEST_F(ListPtrTest, EquivalenceNullptr) {
  Ptr p;

  EXPECT_TRUE(p == nullptr);
  EXPECT_FALSE(p != nullptr);
  EXPECT_TRUE(nullptr == p);
  EXPECT_FALSE(nullptr != p);
}

TEST_F(ListPtrTest, CheckLifetime) {
  bool deleted = false;
  {
    auto* ptr = new DestructionTracker(&deleted);
    ListPtr<DestructionTracker> d(ptr);
    {
      ListPtr<DestructionTracker> b1 = d;
      auto b2 = d;
      EXPECT_EQ(ptr, b1.get());
      EXPECT_EQ(ptr, b2.get());
      EXPECT_EQ(ptr, d.get());
      EXPECT_FALSE(deleted);
    }
    EXPECT_FALSE(deleted);
  }
  EXPECT_TRUE(deleted);
}

TEST(TraitsTest, Ctors) {
  static_assert(std::is_constructible_v<ListPtr<int>, int*>);
  static_assert(std::is_constructible_v<ListPtr<int>, int*, std::default_delete<int>>);
  static_assert(!std::is_constructible_v<ListPtr<int>, double*, std::default_delete<double>>);
  static_assert(!std::is_constructible_v<ListPtr<int>, int*, std::default_delete<double>>);
  static_assert(!std::is_constructible_v<ListPtr<int>, double*, std::default_delete<int>>);

  static_assert(!std::is_constructible_v<ListPtr<DestructionTracker>, DestructionTrackerBase*>);
  static_assert(std::is_constructible_v<ListPtr<DestructionTrackerBase>, DestructionTracker*>);
  static_assert(std::is_constructible_v<
                ListPtr<DestructionTrackerBase>,
                DestructionTracker*,
                std::default_delete<DestructionTracker>>);
}

using DestructionTrackerBaseDeleter = std::default_delete<DestructionTrackerBase>;

TEST(TraitsTest, CopyMoveCtor) {
  static_assert(std::is_constructible_v<ListPtr<const int>, ListPtr<int>>);
  static_assert(!std::is_constructible_v<ListPtr<int>, ListPtr<const int>>);
  static_assert(!std::is_constructible_v<ListPtr<int>, ListPtr<double>>);

  static_assert(!std::is_constructible_v<ListPtr<DestructionTracker>, ListPtr<DestructionTrackerBase>>);
  static_assert(std::is_constructible_v<ListPtr<DestructionTrackerBase>, ListPtr<DestructionTracker>>);
  static_assert(std::is_constructible_v<
                ListPtr<DestructionTrackerBase>,
                ListPtr<DestructionTracker, DestructionTrackerBaseDeleter>>);
}

TEST(TraitsTest, Assignment) {
  static_assert(std::is_assignable_v<ListPtr<const int>, ListPtr<int>>);
  static_assert(!std::is_assignable_v<ListPtr<int>, ListPtr<const int>>);
  static_assert(!std::is_assignable_v<ListPtr<int>, ListPtr<double>>);

  static_assert(!std::is_assignable_v<ListPtr<DestructionTracker>, ListPtr<DestructionTrackerBase>>);
  static_assert(std::is_assignable_v<ListPtr<DestructionTrackerBase>, ListPtr<DestructionTracker>>);
  static_assert(std::is_assignable_v<
                ListPtr<DestructionTrackerBase>,
                ListPtr<DestructionTracker, DestructionTrackerBaseDeleter>>);
}

} // namespace ct::test
