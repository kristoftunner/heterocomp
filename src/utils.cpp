#include "utils.hpp"

void Benchmark::StartMark(const std::string& message)
{
  m_begTimes.push_back(std::chrono::high_resolution_clock::now());
  m_markedMessages.push_back(message);
}

void Benchmark::StopMark()
{
  m_endTimes.push_back(std::chrono::high_resolution_clock::now());
  std::chrono::duration<double> elapsedTime = m_endTimes.back() - m_begTimes.back();
  m_markedTimes.push_back(static_cast<double>(elapsedTime.count()) * 1000);
  std::cout << "time for " << m_markedMessages.back() << "is: " << m_markedTimes.back() << " ms" << std::endl;
}
