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
      if (argv[i] == "-v" || argv[i] == "--verbose") {
        // verbose mode
        verbose = true;
      } else if (argv[i] == "-h" || argv[i] == "--help") {
        std::cout << HelpMsg << std::endl;
        return 0;
      } else {
        // tm definition file
        if (tm_def_path == "") {
          tm_def_path = argv[i];
        }
        // tm input string
        else {
          input = argv[i];
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
  tm.run();

  std::cout << 'TODO' << std::endl;

  return 0;
}