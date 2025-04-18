//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// index_scan_executor.cpp
//
// Identification: src/execution/index_scan_executor.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "execution/executors/index_scan_executor.h"
#include "common/macros.h"

namespace bustub {

/**
 * Creates a new index scan executor.
 * @param exec_ctx the executor context
 * @param plan the index scan plan to be executed
 */
IndexScanExecutor::IndexScanExecutor(ExecutorContext *exec_ctx, const IndexScanPlanNode *plan)
    : AbstractExecutor(exec_ctx) {
  UNIMPLEMENTED("TODO(P3): Add implementation.");
}

void IndexScanExecutor::Init() { UNIMPLEMENTED("TODO(P3): Add implementation."); }

auto IndexScanExecutor::Next(Tuple *tuple, RID *rid) -> bool { UNIMPLEMENTED("TODO(P3): Add implementation."); }

}  // namespace bustub
