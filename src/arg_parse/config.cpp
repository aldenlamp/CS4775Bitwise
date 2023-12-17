#include "config.hpp"

#include <iostream>

using std::string;
using std::vector;

namespace argparse {

void Config::Copy(const Config& other) {
  // parser_ = other.parser_;
  for (vector<BaseArg*>::iterator it = args_.begin(); it != args_.end(); ++it) {
    args_.push_back((*it)->Clone());
  }
}

void Config::Delete() {
  for (vector<BaseArg*>::iterator it = args_.begin(); it != args_.end(); ++it) {
    delete (*it);
  }
}

Config::~Config() { Delete(); }

// Config::Config(const Config& other) : parser_(other.parser_) { Copy(other); }

const Config& Config::operator=(const Config& other) {
  Delete();
  Copy(other);
  return *this;
}

// void Config::AddAllArgs() {}

void Config::OutputHelp() {
  std::cout << "Config parameters for: " << config_name_ << std::endl;
  std::cout << std::setw(kHelpWidthLong) << std::left << "NAME"
            << std::setw(kHelpWidthShort) << std::left << "KEY"
            << std::setw(kHelpWidthShort) << std::left << "DTYPE"
            << std::setw(kHelpWidthShort) << std::left << "DEFAULT"
            << std::setw(kHelpWidthShort) << std::left << "REQUIRED"
            << "DESCRIPTION" << std::endl
            << std::endl;

  for (std::vector<BaseArg*>::iterator it = args_.begin(); it != args_.end();
       ++it) {
    std::cout << *(*it) << "\n";
  }
  std::cout << std::endl;
}

void Config::ListArgs() {
  std::cout << "Args for config: " << config_name_ << "\n";
  for (std::vector<BaseArg*>::iterator it = args_.begin(); it != args_.end();
       ++it) {
    std::cout << (*it)->GetValueStr() << "\n";
  }
  std::cout << std::endl;
}

void Config::ValidateRequired() {
  for (std::vector<BaseArg*>::iterator it = args_.begin(); it != args_.end();
       ++it) {
    (*it)->ValidateRequired();
  }
  // parser_.ValidateUnused();
}

void Config::ParseArgs(ArgParser* parser) {
  parser->SetKnownArgs(GetArgFlags());
  for (std::vector<BaseArg*>::iterator it = args_.begin(); it != args_.end();
       ++it) {
    (*it)->ParseArg(parser);
  }
}

vector<string> Config::GetArgFlags() {
  vector<string> flags = vector<string>();
  for (std::vector<BaseArg*>::iterator it = args_.begin(); it != args_.end();
       ++it) {
    flags.push_back((*it)->GetFlag());
  }
  return flags;
}

}  // namespace argparse