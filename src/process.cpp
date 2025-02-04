#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) { CpuUtilization(); }

int Process::Pid() { return Process::pid_; }

float Process::CpuUtilization() {
  cpu_util_ = LinuxParser::ProcessorUtilization(Process::Pid());
  return cpu_util_;
}

string Process::Command() {
  auto command = LinuxParser::Command(Process::Pid());
  // limiting the command to 50 characters
  command = command.length() > 50 ? command + "..." : command;
  return command;
}

string Process::Ram() { return LinuxParser::Ram(Process::Pid()); }

string Process::User() { return LinuxParser::User(Process::Pid()); }

long int Process::UpTime() {
  return (LinuxParser::UpTime() - LinuxParser::UpTime(Process::Pid()));
}

bool Process::operator<(Process const& a) const {
  return a.cpu_util_ < this->cpu_util_;
}