#include <string>

#include "linux_parser.h"
#include "processor.h"

float Processor::Utilization() {
  auto cpu_measurements = LinuxParser::CpuUtilization();
  float user = std::stof(cpu_measurements[0]);
  float nice = std::stof(cpu_measurements[1]);
  float system = std::stof(cpu_measurements[2]);
  float idle = std::stof(cpu_measurements[3]);
  float iowait = std::stof(cpu_measurements[4]);
  float irq = std::stof(cpu_measurements[5]);
  float softirq = std::stof(cpu_measurements[6]);
  float steal = std::stof(cpu_measurements[7]);

  auto PrevIdle = previdle_ + previowait_;
  auto Idle = idle + iowait;

  auto PrevNonIdle = prevuser_ + prevnice_ + prevsystem_ + previrq_ +
                     prevsoftirq_ + prevsteal_;
  auto NonIdle = user + nice + system + irq + softirq + steal;

  auto PrevTotal = PrevIdle + PrevNonIdle;
  auto Total = Idle + NonIdle;

  // differentiate: actual value minus the previous one
  auto totald = Total - PrevTotal;
  auto idled = Idle - PrevIdle;
  float cpu_percentage = (totald - idled) / totald;

  // assigning to previous values
  previdle_ = idle;
  previowait_ = iowait;
  prevnice_ = nice;
  prevuser_ = user;
  prevsystem_ = system;
  previrq_ = irq;
  prevsoftirq_ = softirq;
  prevsteal_ = steal;

  return cpu_percentage;
}