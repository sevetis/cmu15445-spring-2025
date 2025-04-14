//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// skiplist.cpp
//
// Identification: src/primer/skiplist.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/skiplist.h"
#include <cassert>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <stack>
#include "fmt/core.h"

namespace bustub {

/** @brief Checks whether the container is empty. */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::Empty() -> bool {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  return size_ == 0;
}

/** @brief Returns the number of elements in the skip list. */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::Size() -> size_t {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  return size_;
}

/**
 * @brief Iteratively deallocate all the nodes.
 *
 * We do this to avoid stack overflow when the skip list is large.
 *
 * If we let the compiler handle the deallocation, it will recursively call the destructor of each node,
 * which could block up the the stack.
 */
SKIPLIST_TEMPLATE_ARGUMENTS void SkipList<K, Compare, MaxHeight, Seed>::Drop() {
  for (size_t i = 0; i < MaxHeight; i++) {
    auto curr = std::move(header_->links_[i]);
    while (curr != nullptr) {
      // std::move sets `curr` to the old value of `curr->links_[i]`,
      // and then resets `curr->links_[i]` to `nullptr`.
      curr = std::move(curr->links_[i]);
    }
  }
}

/**
 * @brief Removes all elements from the skip list.
 *
 * Note: You might want to use the provided `Drop` helper function.
 */
SKIPLIST_TEMPLATE_ARGUMENTS void SkipList<K, Compare, MaxHeight, Seed>::Clear() {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");]
  size_ = 0;
  Drop();
}

/**
 * @brief Inserts a key into the skip list.
 *
 * Note: `Insert` will not insert the key if it already exists in the skip list.
 *
 * @param key key to insert.
 * @return true if the insertion is successful, false if the key already exists.
 */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::Insert(const K &key) -> bool {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  if (Contains(key))
    return false;

  auto cur_ptr = header_;
  auto nodes = std::stack<std::shared_ptr<SkipNode>>();

  for (int i = MaxHeight - 1; i >= 0; --i) {
    while (true) {
      auto nxt_ptr = (*cur_ptr).Next(i);
      if (nxt_ptr == nullptr or compare_(key, (*nxt_ptr).Key()))
        break;
      cur_ptr = nxt_ptr;
    }
    if (nodes.empty() or cur_ptr != nodes.top())
      nodes.push(cur_ptr);
  }

  auto height = RandomHeight(), level = 0UL;
  auto to_insert = std::make_shared<SkipNode>(height, key);
  while (level < height) {
    auto node = nodes.top();
    nodes.pop();

    while (level < std::min(height, (*node).Height())) {
      auto nxt_node = (*node).Next(level);
      (*node).SetNext(level, to_insert);
      (*to_insert).SetNext(level, nxt_node);
      ++level;
    }
  }

  ++size_;
  return true;
}

/**
 * @brief Erases the key from the skip list.
 *
 * @param key key to erase.
 * @return bool true if the element got erased, false otherwise.
 */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::Erase(const K &key) -> bool {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  if (not Contains(key))
    return false;

  auto cur_ptr = header_;
  auto nodes = std::stack<std::shared_ptr<SkipNode>>();
  std::shared_ptr<SkipNode> to_erase;

  for (int i = MaxHeight - 1; i >= 0; --i) {
    while (true) {
      auto nxt_ptr = (*cur_ptr).Next(i);
      if (nxt_ptr == nullptr)
        break;

      auto nxt_key = (*nxt_ptr).Key();
      if (compare_(key, nxt_key)) break;
      else if (compare_(nxt_key, key)) {
        cur_ptr = nxt_ptr;
      } else {
        to_erase = nxt_ptr;
        break;
      }
    }

    if (nodes.empty() or cur_ptr != nodes.top())
      nodes.push(cur_ptr);
  }

  auto height = (*to_erase).Height(), level = 0UL;
  while (level < height) {
    auto node = nodes.top();
    nodes.pop();

    while (level < std::min(height, (*node).Height())) {
      auto nxt_node = (*to_erase).Next(level);
      (*to_erase).SetNext(level, nullptr);
      (*node).SetNext(level, nxt_node);
      ++level;
    }
  }

  --size_;
  return true;
}

/**
 * @brief Checks whether a key exists in the skip list.
 *
 * @param key key to look up.
 * @return bool true if the element exists, false otherwise.
 */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::Contains(const K &key) -> bool {
  // Following the standard library: Key `a` and `b` are considered equivalent if neither compares less
  // than the other: `!compare_(a, b) && !compare_(b, a)`.
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  auto cur_ptr = header_;

  for (int i = MaxHeight - 1; i >= 0; --i) {
    while (true) {
      auto nxt_ptr = (*cur_ptr).Next(i);
      if (nxt_ptr == nullptr)
        break;

      auto nxt_key = (*nxt_ptr).Key();
      if (compare_(key, nxt_key)) break;
      else if (not compare_(nxt_key, key))
        return true;
      cur_ptr = nxt_ptr;
    }
  }

  return false;
}

/**
 * @brief Prints the skip list for debugging purposes.
 *
 * Note: You may modify the functions in any way and the output is not tested.
 */
SKIPLIST_TEMPLATE_ARGUMENTS void SkipList<K, Compare, MaxHeight, Seed>::Print() {
  auto node = header_->Next(LOWEST_LEVEL);
  while (node != nullptr) {
    fmt::println("Node {{ key: {}, height: {} }}", node->Key(), node->Height());
    node = node->Next(LOWEST_LEVEL);
  }
}

/**
 * @brief Generate a random height. The height should be cappped at `MaxHeight`.
 * Note: we implement/simulate the geometric process to ensure platform independence.
 */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::RandomHeight() -> size_t {
  // Branching factor (1 in 4 chance), see Pugh's paper.
  static constexpr unsigned int branching_factor = 4;
  // Start with the minimum height
  size_t height = 1;
  while (height < MaxHeight && (rng_() % branching_factor == 0)) {
    height++;
  }
  return height;
}

/**
 * @brief Gets the current node height.
 */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::SkipNode::Height() const -> size_t {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  return links_.size();
}

/**
 * @brief Gets the next node by following the link at `level`.
 *
 * @param level index to the link.
 * @return std::shared_ptr<SkipNode> the next node, or `nullptr` if such node does not exist.
 */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::SkipNode::Next(size_t level) const
    -> std::shared_ptr<SkipNode> {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  if (level >= links_.size())
      throw "level " + std::to_string(level) + " out of range";
  return links_[level];
}

/**
 * @brief Set the `node` to be linked at `level`.
 *
 * @param level index to the link.
 */
SKIPLIST_TEMPLATE_ARGUMENTS void SkipList<K, Compare, MaxHeight, Seed>::SkipNode::SetNext(
    size_t level, const std::shared_ptr<SkipNode> &node) {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  if (level >= links_.size())
      throw "level " + std::to_string(level) + " out of range";
  links_[level] = node;
}

/** @brief Returns a reference to the key stored in the node. */
SKIPLIST_TEMPLATE_ARGUMENTS auto SkipList<K, Compare, MaxHeight, Seed>::SkipNode::Key() const -> const K & {
  // UNIMPLEMENTED("TODO(P0): Add implementation.");
  return key_;
}

// Below are explicit instantiation of template classes.
template class SkipList<int>;
template class SkipList<std::string>;
template class SkipList<int, std::greater<>>;
template class SkipList<int, std::less<>, 8>;

}  // namespace bustub
