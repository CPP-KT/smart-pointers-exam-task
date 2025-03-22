#pragma once

#include <memory>

namespace ct {

template <typename T, typename Deleter = std::default_delete<T>>
class ListPtr {
public:
  ListPtr();

  ~ListPtr();

  ListPtr(std::nullptr_t);

  explicit ListPtr(T* ptr);

  ListPtr(T* ptr, Deleter deleter);

  ListPtr(const ListPtr& other);

  template <typename Y, typename D>
  ListPtr(const ListPtr<Y, D>& other, T* ptr);

  template <typename Y, typename D>
  ListPtr(ListPtr<Y, D>&& other, T* ptr);

  ListPtr(ListPtr&& other);

  template <typename Y, typename D>
  ListPtr(const ListPtr<Y, D>& other);

  template <typename Y, typename D>
  ListPtr(ListPtr<Y, D>&& other);

  ListPtr& operator=(const ListPtr& other);

  ListPtr& operator=(ListPtr&& other);

  template <typename Y, typename D>
  ListPtr& operator=(const ListPtr<Y, D>& other);

  template <typename Y, typename D>
  ListPtr& operator=(ListPtr<Y, D>&& other);

  T* get() const;

  explicit operator bool() const;

  T& operator*() const;

  T* operator->() const;

  std::size_t useCount() const;

  void reset();

  void reset(T* new_ptr);

  T* release();

  friend bool operator==(const ListPtr& lhs, const ListPtr& rhs);

  friend bool operator!=(const ListPtr& lhs, const ListPtr& rhs);
};

template <typename T, typename... Args>
ListPtr<T> makeListPtr(Args&&... args);

} // namespace ct
