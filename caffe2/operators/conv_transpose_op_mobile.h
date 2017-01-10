#ifndef CAFFE2_OPERATORS_CONV_TRANSPOSE_MOBILE_OP_H_
#define CAFFE2_OPERATORS_CONV_TRANSPOSE_MOBILE_OP_H_

#include "caffe2/core/common.h"

#ifndef CAFFE2_MOBILE
#error "mobile build state not defined"
#endif

#if CAFFE2_MOBILE

#include "caffe2/core/context.h"
#include "caffe2/core/operator.h"
#include "caffe2/operators/conv_transpose_unpool_op_base.h"

namespace caffe2 {

template <typename T, class Context>
class ConvTransposeMobileOp final : public ConvTransposeUnpoolBase<Context> {
 public:
  USE_CONV_TRANSPOSE_UNPOOL_BASE_FUNCTIONS(Context);
  ConvTransposeMobileOp(const OperatorDef& operator_def, Workspace* ws)
      : ConvTransposeUnpoolBase<Context>(operator_def, ws) {
    OPERATOR_NEEDS_FEATURE(order_ == StorageOrder::NCHW,
                           "Only NCHW order is supported right now.");
    OPERATOR_NEEDS_FEATURE(pad_l_ == 0,
                           "operator does not handle row width padding");
    OPERATOR_NEEDS_FEATURE(pad_r_ == 0,
                           "operator does not handle row width padding");
    OPERATOR_NEEDS_FEATURE(stride_w_ <= 4, "stride width must be <= 4");
  }

  bool RunOnDeviceWithOrderNCHW() override;
  bool RunOnDeviceWithOrderNHWC() override;

 private:
  // For the multithreaded implementation, this stores a per-worker
  // thread buffer for a tile of Y. There is one of these buffers for
  // each worker thread that is not the main thread.
  std::vector<Tensor<Context>> threadYBuffers_;

  // For the multithreaded implementation, this stores a per-worker
  // thread buffer for the gemm output. There is one of these buffers
  // for all worker threads, including the main thread.
  std::vector<Tensor<Context>> threadColBuffers_;

  // Input: X, W, b
  // Output: Y
  INPUT_TAGS(INPUT, FILTER, BIAS);
};

} // namespace caffe2

#endif // CAFFE2_MOBILE

#endif // CAFFE2_OPERATORS_CONV_TRANSPOSE_MOBILE_OP_H_