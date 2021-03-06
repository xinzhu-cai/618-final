#ifndef _LOCK_QUEUE_H_
#define _LOCK_QUEUE_H_

#include <atomic>
#include <mutex>
#include "config.h"
#include "queue.h"

using namespace std;

template <typename T>
class LockQueue : public Queue<T> {
 public:
  LockQueue() : head_(nullptr), tail_(nullptr) {
    count_.store(0, std::memory_order_relaxed);
  }

  virtual void Push(const T& value) {
    lock_guard<mutex> lock(mu_);
    if (tail_ == nullptr) {
      tail_ = head_ = new Node(value);
    } else {
      tail_->next_ = new Node(value);
      tail_ = tail_->next_;
    }
    count_.store(count_.load(std::memory_order_relaxed) + 1,
                 std::memory_order_relaxed);
  }

  virtual void Push(T&& value) {
    lock_guard<mutex> lock(mu_);
    if (tail_ == nullptr) {
      tail_ = head_ = new Node(value);
    } else {
      tail_->next_ = new Node(value);
      tail_ = tail_->next_;
    }
    count_.store(count_.load(std::memory_order_relaxed) + 1,
                 std::memory_order_relaxed);
  }

  virtual optional<T> Pop() {
    lock_guard<mutex> lock(mu_);
    std::optional<T> optval;
    if (count_.load(std::memory_order_relaxed) == 0) return optval;
    optval = move(head_->value_);
    auto old_head = head_;
    head_ = head_->next_;
    delete old_head;
    if (head_ == nullptr) tail_ = nullptr;
    count_.store(count_.load(std::memory_order_relaxed) - 1,
                 std::memory_order_relaxed);
    return optval;
  }

 private:
  struct ALIGNED Node {
    T value_;
    Node* next_;

    Node(const T& value) : value_(value), next_(nullptr) {}
    Node(T&& value) : value_(value), next_(nullptr) {}
  };

  ALIGNED Node* head_;
  ALIGNED Node* tail_;

  ALIGNED mutex mu_;
  ALIGNED atomic<int> count_;
};

#endif  // _LOCK_QUEUE_H_
