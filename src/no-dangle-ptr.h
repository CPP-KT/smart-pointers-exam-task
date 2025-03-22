#pragma once

template <typename T>
class NoDanglePtr {
public:
  NoDanglePtr();

  ~NoDanglePtr();

  explicit NoDanglePtr(T* ptr);

  NoDanglePtr(const NoDanglePtr& other);

  NoDanglePtr(NoDanglePtr&& other);

  NoDanglePtr& operator=(const NoDanglePtr& other);

  NoDanglePtr& operator=(NoDanglePtr&& other);

  T* get() const;

  operator T*() const;

  T& operator*() const;

  T* operator->() const;

  explicit operator bool() const;
};
