#pragma once

#include "CL/cl.h"
#include "common.hpp"

const char* TranslateOpenCLError(cl_int errorCode);

class Benchmark {
public:
  void StartMark();
  void StopMark();
private:
  std::vector<double> m_markedTimes;
  std::vector<std::string> m_markedMessages;
  std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> m_begTimes;
  std::vector<std::chrono::time_point<std::chrono::high_resolution_clock>> m_endTimes;
};