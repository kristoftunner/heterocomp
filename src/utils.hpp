#pragma once

#include "CL/cl.h"
#include "common.hpp"

class CLUtils {
public:
  static void CheckError(int error);
};

class Benchmark {
public:
  void StartMark(const std::string& message);
  void StopMark();
private:
  std::vector<double> m_markedTimes;
  std::vector<std::string> m_markedMessages;
  std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> m_begTimes;
  std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> m_endTimes;
};