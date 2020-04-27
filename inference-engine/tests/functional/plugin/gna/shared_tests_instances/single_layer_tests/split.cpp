// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include <vector>

#include "single_layer_tests/split.hpp"
#include "common_test_utils/test_constants.hpp"

using namespace LayerTestsDefinitions;

namespace {
// Common params
const std::vector<InferenceEngine::Precision> inputPrecisions = {
        InferenceEngine::Precision::FP32,
        InferenceEngine::Precision::U8
// TODO: Issue: 26570
//      InferenceEngine::Precision::I8
};

const std::vector<InferenceEngine::Precision> netPrecisions = {
        InferenceEngine::Precision::FP32,
        InferenceEngine::Precision::FP16
};

// TODO: Issue:  26411
INSTANTIATE_TEST_CASE_P(DISABLED_NumSplitsCheck, SplitLayerTest,
                        ::testing::Combine(
                                ::testing::Values(1),
                                ::testing::Values(0, 1),
                                ::testing::ValuesIn(inputPrecisions),
                                ::testing::ValuesIn(netPrecisions),
                                ::testing::Values(std::vector<size_t >({30, 30})),
                                ::testing::Values(CommonTestUtils::DEVICE_GNA)),
                        SplitLayerTest::getTestCaseName);

}  // namespace