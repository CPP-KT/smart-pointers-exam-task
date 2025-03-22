#pragma once

template <typename T>
struct TrackingDeleter {
  explicit TrackingDeleter(bool* deleted)
      : deleted(deleted) {}

  TrackingDeleter(const TrackingDeleter&) = delete;
  TrackingDeleter(TrackingDeleter&&) = default;

  void operator()(T* object) {
    *deleted = true;
    delete object;
  }

private:
  bool* deleted;
};

struct DestructionTrackerBase {
  DestructionTrackerBase() = default;

  DestructionTrackerBase(const DestructionTrackerBase&) = delete;
  DestructionTrackerBase& operator=(const DestructionTrackerBase&) = delete;

  DestructionTrackerBase(DestructionTrackerBase&&) = delete;
  DestructionTrackerBase& operator=(DestructionTrackerBase&&) = delete;
};

struct DestructionTracker : DestructionTrackerBase {
  explicit DestructionTracker(bool* deleted)
      : deleted(deleted) {}

  ~DestructionTracker() {
    *deleted = true;
  }

private:
  bool* deleted;
};
