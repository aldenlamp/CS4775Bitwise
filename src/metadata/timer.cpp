#include "timer.hpp"

#include <iomanip>

using std::ostream;
using std::setw;
using std::string;
using std::unordered_map;
using std::chrono::duration;
using std::chrono::high_resolution_clock;

namespace metadata {

typedef unordered_map<string, time_point>::const_iterator map_it;

void Timer::Start(const string& timer_name) {
  time_point start_time = high_resolution_clock::now();
  start_times_[timer_name] = start_time;
}

void Timer::End(const string& timer_name) {
  time_point end_time = high_resolution_clock::now();
  end_times_[timer_name] = end_time;
}

double Timer::GetTime(const string& timer_name) const {
  map_it start = start_times_.find(timer_name);
  map_it end = end_times_.find(timer_name);
  if (start == start_times_.end() || end == end_times_.end()) return -1;

  duration<double, std::milli> time_delta = end->second - start->second;
  return time_delta.count();
}

ostream& operator<<(ostream& os, const Timer& timer) {
  os << "Times: " << std::endl;
  map_it it;

  constexpr int spacing = 40;

  for (it = timer.end_times_.begin(); it != timer.end_times_.end(); ++it) {
    string time_name = it->first;
    os << "\t " << std::setw(spacing) << std::left << time_name << ":  "
       << timer.GetTime(time_name) << std::endl;
  }
  return os;
}

ostream& Timer::OutputPartition(ostream& os, const string& partition) {
  os << "Times in partition " << partition << ": " << std::endl;
  map_it it;

  constexpr int spacing = 40;

  for (it = end_times_.begin(); it != end_times_.end(); ++it) {
    string time_name = it->first;
    os << "::>\t" << (partition.compare("") != 0 ? (partition + ":") : "")
       << time_name << ":" << GetTime(time_name) << std::endl;
  }
  return os;
}

}  // namespace metadata