#ifndef SPARSE_METADATA_TIMER_H_
#define SPARSE_METADATA_TIMER_H_

#include <chrono>
#include <iostream>
#include <string>
#include <unordered_map>

namespace metadata {

namespace timernames {

const std::string kInputTimer = "Input";
const std::string kCorrelationTimer = "Calculate Grm";
const std::string kTraceEstimateTimer = "Calculate Estimate Trace";
const std::string kTraceTimer = "Calculate Exact Trace";
const std::string kTraceAlphaTimer = "Calculate Exact Alpha Trace";

}  // namespace timernames

typedef std::chrono::high_resolution_clock::time_point time_point;

class Timer {
 private:
  std::unordered_map<std::string, time_point> start_times_;
  std::unordered_map<std::string, time_point> end_times_;

 public:
  /**
   * @brief Start a timer corresponding to the given name. If there already
   *   exists a timer with the same name, it will restart that timer.
   *
   * @param timer_name The name of the timer to start
   */
  void Start(const std::string& timer_name);

  /**
   * @brief End a timer corresponding to the given name. If there already
   *   exists an ended timer with the same name, it will now represent the
   *   time elapsed since the most recent start.
   *
   * @param timer_name The name of the timer to end
   */
  void End(const std::string& timer_name);

  /**
   * @brief Get the elapsed time of the timer with given name in
   *   milliseconds.
   *
   * @param timer_name The name of the timer to track
   * @return double
   */
  double GetTime(const std::string& timer_name) const;

  /**
   * @brief Gets the output stream of time times given a partition
   *
   * @param partition a string representing which partition this timer
   *   belongs to
   * @return std::ostream&
   */
  std::ostream& OutputPartition(std::ostream& os, const std::string& partition);

  /**
   * @brief Outputs all the stored times to the output stream
   *
   * @param os the stream to input
   * @param timer the timer to output
   * @return std::ostream&
   */
  friend std::ostream& operator<<(std::ostream& os, const Timer& timer);
};

}  // namespace metadata

#endif