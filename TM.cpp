#include "TM.h"

#include <fstream>
#include <iostream>

using namespace std;

/*
 * FLA21Fall NJUCS
 * LYT - lyt-42@outlook.com
 * Dec 22, 2021
 *
 * Simulate Turing Machine
 * 1. [ ] Read input from file and generate TM
 * 2. [ ] Run TM (normal, verbose)
 * 3. [ ] Multi-type TM
 */

map<char, vector<string>> type2invalidChar = []() -> auto {
  map<char, vector<string>> tmp;
  vector<string> s = {" ", "\t\n", ",", ";", "{", "}", "*", "_"};
  vector<string> g = {" ", "\t\n", ",", ";", "{", "}", "*"};
  tmp['S'] = s;
  tmp['g'] = g;
  return tmp;
}
();

// ======= Helper Functions ========
set<string> split(string &str, char delimiter) {
  // given "item1<delimiter>item2<delimiter>item3"
  set<string> result;
  string tmp;
  while (str.length() > 0) {
    tmp = str.substr(0, str.find(delimiter));
    result.insert(tmp);
    str.erase(0, str.find(delimiter));
  }
  return result;
}

int TuringMachine::init_tm(string &TMDef) {
  ifstream tm_def;
  tm_def.open(TMDef);
  if (!tm_def.is_open()) {
    std::cerr << "Error: cannot open TM define file at" << TMDef << std::endl;
    return -1;
  }

  string tmp_line;
  string erro_msg;
  int line_num = 0;
  while (getline(tm_def, tmp_line)) {
    line_num++;

    // remove commnets and empty lines
    if ((tmp_line.rfind(';', 0) == 0) || (tmp_line.length() <= 1)) {
      continue;
    }

    if (tmp_line.rfind('#', 0) == 0) {  // TM symbols
      switch (tmp_line[1]) {
        case 'Q': {
          size_t lBrack = tmp_line.find('{');
          size_t rBrack = tmp_line.find('}');
          if (lBrack == string::npos || rBrack == string::npos) {
            erro_msg = "Q not including by { }";
            goto ErroHandle;
          }
          tmp_line = tmp_line.substr(lBrack, rBrack - lBrack);
          if (tmp_line.find(' ') != string::npos) {
            erro_msg = "whitespace in Q def";
            goto ErroHandle;
          }

          this->states = split(tmp_line, ',');
        } break;

        case 'S': {
          size_t lBrack = tmp_line.find('{');
          size_t rBrack = tmp_line.find('}');
          if (lBrack == string::npos || rBrack == string::npos) {
            erro_msg = "S not included by { }";
            goto ErroHandle;
          }
          tmp_line = tmp_line.substr(lBrack, rBrack - lBrack);
          for (string delim : type2invalidChar['S']) {
            if (tmp_line.find(delim) != string::npos) {
              erro_msg = "invalid char in S def";
              goto ErroHandle;
            }
          }

          this->inputSymbols = split(tmp_line, ',');
        } break;

        case 'G': {
          size_t lBrack = tmp_line.find('{');
          size_t rBrack = tmp_line.find('}');
          if (lBrack == string::npos || rBrack == string::npos) {
            erro_msg = "S not included by { }";
            goto ErroHandle;
          }
          tmp_line = tmp_line.substr(lBrack, rBrack - lBrack);
          for (string delim : type2invalidChar['G']) {
            if (tmp_line.find(delim) != string::npos) {
              erro_msg = "invalid char in G def";
              goto ErroHandle;
            }
          }

          this->tapeSymbols = split(tmp_line, ',');
        } break;
        case 'q': {
          if (tmp_line[2] != '0' || tmp_line.substr(3, 3) != " = ") {
            erro_msg = "Syntex Error: Illegal def of state state";
            goto ErroHandle;
          }

          string q0(tmp_line, 6, tmp_line.length() - 5);
          this->startState = q0;
        } break;
        case 'B':
          break;
        case 'F':
          break;
        case 'N':
          break;
        default:
          goto ErroHandle;
      }
    } else {  // transition
    }
  }

ErroHandle:
  std::cerr << "Error: syntax error at line" << line_num << std::endl;

  if (this->verbose) {
    std::cerr << "In line" << tmp_line << endl;
  }
  exit(-1);
}
