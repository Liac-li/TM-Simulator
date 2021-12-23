#include <iostream>
#include <string>

#include "TM.h"

int main(int argc, char* argv[]) {
  // preprocess args and pass to TM class

  std::string HelpMsg = "usage: turing [-v|--verbose] [-h|--help] <tm> <input>";

  bool verbose = false;
  std::string tm_def_path = "";
  std::string input = "";

  if (argc == 1) {
    std::cout << HelpMsg << std::endl;
    return 0;
  } else {
    for (int i = 1; i < argc; i++) {
      std::string arg = argv[i];
      if (arg == "-v" || arg == "--verbose") {
        // verbose mode
        verbose = true;
      } else if (arg == "-h" || arg == "--help") {
        std::cout << HelpMsg << std::endl;
        return 0;
      } else {
        // tm definition file
        if (tm_def_path == "") {
          tm_def_path = arg;
        }
        // tm input string
        else {
          input = arg;
        }
      }
    }
  }

  if (tm_def_path == "" || input == "") {
    std::cerr << "Error: no tm def file at" << tm_def_path << "or empty input"
              << std::endl;
    return 0;
  }

  TuringMachine tm = TuringMachine(tm_def_path, verbose);
  // tm.displayTMDef();
  tm.run(input);

  std::cout << "TODO" << std::endl;

  return 0;
}