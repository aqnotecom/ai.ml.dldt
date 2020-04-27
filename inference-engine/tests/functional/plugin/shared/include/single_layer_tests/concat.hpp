// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <memory>

#include "functional_test_utils/layer_test_utils.hpp"
#include "ngraph_functions/builders.hpp"
#include "ngraph_functions/utils/ngraph_helpers.hpp"

namespace LayerTestsDefinitions {

using concatParamsTuple = typename std::tuple<
        size_t,                            // Concat axis
        std::vector<std::vector<size_t>>,  // Input shapes
        InferenceEngine::Precision,        // Input precision
        InferenceEngine::Precision,        // Network precision
        std::string>;                      // Device name

class ConcatLayerTest
        : public LayerTestsUtils::LayerTestsCommonClass<concatParamsTuple> {
public:
    static std::string getTestCaseName(const testing::TestParamInfo<concatParamsTuple> &obj);

protected:
    void SetUp() override;
};

}  // namespace LayerTestsDefinitions