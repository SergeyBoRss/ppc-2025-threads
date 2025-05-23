#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>

#include "core/perf/include/perf.hpp"
#include "core/task/include/task.hpp"
#include "tbb/kozlova_e_contrast_enhancement/include/ops_tbb.hpp"

namespace {
std::vector<uint8_t> GenerateVector(int length);

std::vector<uint8_t> GenerateVector(int length) {
  std::vector<uint8_t> vec;
  vec.reserve(length);
  for (int i = 0; i < length; ++i) {
    vec.push_back(rand() % 256);
  }
  return vec;
}
}  // namespace

TEST(kozlova_e_contrast_enhancement_tbb, test_pipeline_run) {
  constexpr int kSize = 19875000;
  size_t width = 7500;
  size_t height = 2650;
  // Create data
  std::vector<uint8_t> in = GenerateVector(kSize);
  std::vector<uint8_t> out(kSize, 0);

  // Create task_data
  auto task_data_omp = std::make_shared<ppc::core::TaskData>();
  task_data_omp->inputs.emplace_back(in.data());
  task_data_omp->inputs_count.emplace_back(in.size());
  task_data_omp->inputs_count.emplace_back(width);
  task_data_omp->inputs_count.emplace_back(height);
  task_data_omp->outputs.emplace_back(out.data());
  task_data_omp->outputs_count.emplace_back(out.size());

  // Create Task
  auto test_task_omp = std::make_shared<kozlova_e_contrast_enhancement_tbb::TestTaskTBB>(task_data_omp);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_omp);
  perf_analyzer->PipelineRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);

  uint8_t min_value = *std::ranges::min_element(in);
  uint8_t max_value = *std::ranges::max_element(in);

  for (size_t i = 0; i < in.size(); ++i) {
    int expected = static_cast<int>(((in[i] - min_value) / (double)(max_value - min_value)) * 255);
    expected = std::clamp(expected, 0, 255);
    EXPECT_EQ(out[i], static_cast<uint8_t>(expected));
  }
}

TEST(kozlova_e_contrast_enhancement_tbb, test_task_run) {
  constexpr int kSize = 19875000;
  size_t width = 7500;
  size_t height = 2650;
  // Create data
  std::vector<uint8_t> in = GenerateVector(kSize);
  std::vector<uint8_t> out(kSize, 0);

  // Create task_data
  auto task_data_omp = std::make_shared<ppc::core::TaskData>();
  task_data_omp->inputs.emplace_back(in.data());
  task_data_omp->inputs_count.emplace_back(in.size());
  task_data_omp->inputs_count.emplace_back(width);
  task_data_omp->inputs_count.emplace_back(height);
  task_data_omp->outputs.emplace_back(out.data());
  task_data_omp->outputs_count.emplace_back(out.size());

  // Create Task
  auto test_task_omp = std::make_shared<kozlova_e_contrast_enhancement_tbb::TestTaskTBB>(task_data_omp);

  // Create Perf attributes
  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  // Create and init perf results
  auto perf_results = std::make_shared<ppc::core::PerfResults>();

  // Create Perf analyzer
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_omp);
  perf_analyzer->TaskRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);

  uint8_t min_value = *std::ranges::min_element(in);
  uint8_t max_value = *std::ranges::max_element(in);

  for (size_t i = 0; i < in.size(); ++i) {
    int expected = static_cast<int>(((in[i] - min_value) / (double)(max_value - min_value)) * 255);
    expected = std::clamp(expected, 0, 255);
    EXPECT_EQ(out[i], static_cast<uint8_t>(expected));
  }
}