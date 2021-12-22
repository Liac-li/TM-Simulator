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

map<char, vector<string>> type2invalidChar = []() -> map<char, vector<string>> {
  map<char, vector<string>> tmp;
  vector<string> s = {" ", "\t\n", ",", ";", "{", "}", "*", "_"};
  vector<string> g = {" ", "\t\n", ",", ";", "{", "}", "*"};
  tmp['S'] = s;
  tmp['g'] = g;
  return tmp;
}();

// ======= Helper Functions ========
set<string> split(string &str, char delimiter) {
  /*
   * read string and spilt to set with given delimiter
   * @param str string only have target items, without blank, space
   */
  set<string> result;
  str += delimiter;

  string tmp;
  while (str.length() > 0) {
    tmp = str.substr(0, str.find(delimiter));
    result.insert(tmp);
    str.erase(0, tmp.length() + 1);
  }
  return result;
}

string removeComment(string &str) {
  /*
   * remove all string behind ;
   */
  const string WHITESPACE = " \r\n\t";
  string res;
  res = str.substr(0, str.find(';'));

  size_t end = res.find_last_not_of(WHITESPACE);
  return (end == string::npos) ? "" : res.substr(0, end + 1);
}

// ======== TuringMachine Method ==========

int TuringMachine::init_tm(string &TMDef) {
  /*
   * Given TM defination file and decode it to class TuringMachine
   * @param TMDef path to target file
   */
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
    tmp_line = removeComment(tmp_line);
    if (tmp_line.length() < 1) {
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
          tmp_line = tmp_line.substr(lBrack + 1, rBrack - lBrack - 1);
          if (tmp_line.find(' ') != string::npos) {
            erro_msg = "whitespace in Q def";
            goto ErroHandle;
          }
          // cout << tmp_line << endl;

          this->states = split(tmp_line, ',');
        } break;

        case 'S': {
          size_t lBrack = tmp_line.find('{');
          size_t rBrack = tmp_line.find('}');
          if (lBrack == string::npos || rBrack == string::npos) {
            erro_msg = "S not included by { }";
            goto ErroHandle;
          }
          tmp_line = tmp_line.substr(lBrack + 1, rBrack - lBrack - 1);
          // for (string delim : type2invalidChar['S']) {
          //   if (tmp_line.find(delim) != string::npos) {
          //     erro_msg = "invalid char in S def" + delim;
          //     goto ErroHandle;
          //   }
          // }

          set<string> tmp_set = split(tmp_line, ',');
          for (string symbol : tmp_set) {
            for (string invalidStr : type2invalidChar['S']) {
              if (tmp_line.find(invalidStr) != string::npos) {
                erro_msg = "illegal str in S def: " + invalidStr;
                goto ErroHandle;
              }
            }
          }

          this->inputSymbols = tmp_set;
        } break;

        case 'G': {
          size_t lBrack = tmp_line.find('{');
          size_t rBrack = tmp_line.find('}');
          if (lBrack == string::npos || rBrack == string::npos) {
            erro_msg = "S not included by { }";
            goto ErroHandle;
          }
          tmp_line = tmp_line.substr(lBrack + 1, rBrack - lBrack - 1);
          // for (string delim : type2invalidChar['G']) {
          //   if (tmp_line.find(delim) != string::npos) {
          //     erro_msg = "invalid char in G def";
          //     goto ErroHandle;
          //   }
          // }
          set<string> tmp_set = split(tmp_line, ',');
          for (string symbol : tmp_set) {
            for (string invalidStr : type2invalidChar['S']) {
              if (tmp_line.find(invalidStr) != string::npos) {
                erro_msg = "illegal str in G def: " + invalidStr;
                goto ErroHandle;
              }
            }
          }

          this->tapeSymbols = tmp_set;
        } break;

        case 'q': {
          if (tmp_line[2] != '0' || tmp_line.substr(3, 3) != " = ") {
            erro_msg = "Syntex Error: Illegal def of state state";
            goto ErroHandle;
          }

          string q0(tmp_line, 6, tmp_line.length() - 5);
          this->startState = q0;
        } break;

        case 'B': {
          if (tmp_line.substr(2, 3) != " = ") {
            erro_msg = "Syntex Error: Illegal def of blank state" +
                       tmp_line.substr(3, 3);
            goto ErroHandle;
          }
          string blank(tmp_line, 5, 1);
          this->blankSymbol = blank;
        } break;

        case 'F': {
          size_t lBrack = tmp_line.find('{');
          size_t rBrack = tmp_line.find('}');
          if (lBrack == string::npos || rBrack == string::npos) {
            erro_msg = "Final set should included by { }";
            goto ErroHandle;
          }
          tmp_line = tmp_line.substr(lBrack + 1, rBrack - lBrack - 1);
          if (tmp_line.find(' ') != string::npos) {
            erro_msg = "whitespace in Final set def";
            goto ErroHandle;
          }

          set<string> tmp_set = split(tmp_line, ',');

          for (string state : tmp_set) {
            if (this->states.find(state) == this->states.end()) {
              erro_msg = "illegal state in Final states set: " + state;
              goto ErroHandle;
            }
          }

          this->finalStates = tmp_set;
        } break;

        case 'N': {
          if (tmp_line.substr(2, 3) != " = ") {
            erro_msg = "Syntex Error: illegal in tape Num def";
            goto ErroHandle;
          }

          tmp_line = tmp_line[5];
          this->numOfTapes = stoi(tmp_line);
        } break;

        default:
          erro_msg = "Syntex Error: Undefined symbol type start with #";
          goto ErroHandle;
      }
    } else {  // transition
      int ret_code = init_delta(tmp_line);
      if (ret_code != 0) {
        erro_msg = "Running Error: transition function def";
        goto ErroHandle;
      }
    }
  }

  return 0;

ErroHandle:
  cerr << erro_msg << endl;

  if (this->verbose) {
    std::cerr << "Error: syntax error at line: " << line_num << std::endl;
    std::cerr << "In line: " << tmp_line << endl;
  }
  exit(-1);
}

int TuringMachine::init_delta(string &trans_def) {
  /*
   * Initialize Transition function into map<vector<keys>, vector<values>> given
   * def line key: cur_state + input value: new_tape_symbole + moving direction
   * + next state
   */
  vector<string> items;
  trans_def += ' ';

  while (trans_def.length() > 0) {
    string tmp = trans_def.substr(0, trans_def.find(' '));
    items.push_back(tmp);
    trans_def.erase(0, tmp.length() + 1);
  }

  vector<string> keys;
  vector<string> vals;
  string erro_msg = "Syntex Error: [Transition] ";
  if (items.size() != 5) {
    erro_msg += "items size not equal to 5, get" + items.size();
    goto ErrorHandle;
  }

  for (int idx = 0; idx < 5; idx++) {
    string item = items[idx];

    if (1 <= idx && idx <= 3) {
      if (item.length() != this->numOfTapes) {
        erro_msg += "not equal to tape num\n";
        erro_msg += "expect: " + to_string(this->numOfTapes) +
                    " get: " + to_string(item.length());
        erro_msg += "[" + item + "]";
        goto ErrorHandle;
      }
    } else {
      if (this->states.find(item) == this->states.end()) {
        erro_msg += "illegal TM state [" + item + "]";
        goto ErrorHandle;
      }
    }

    if (idx < 2) {
      keys.push_back(item);
    } else {
      vals.push_back(item);
    }
  }

  this->transitionFunction[keys] = vals;
  return 0;

ErrorHandle:
  cerr << erro_msg << endl;
  if (this->verbose) {
    cerr << "items:";
    for (string item : items) {
      cerr << item << ' ';
    }
    cerr << endl;
  }
  return -1;
}

void TuringMachine::displayTMDef() {
  vector<set<string>> items = {this->states, this->inputSymbols,
                               this->tapeSymbols, this->finalStates};
  vector<string> names = {"states", "intputSymbol", "tapeSymbol",
                          "finalStates"};
  for (int idx = 0; idx < 4; idx++) {
    cout << names[idx] << " : " << endl;
    for (string item : items[idx]) {
      cout << item << ' ';
    }
    cout << endl;
  }

  cout << "startState: " << this->startState << endl;
  cout << "blankSymbol: " << this->blankSymbol << endl;
  cout << "tape num: " << this->numOfTapes << endl;

  vector<string> keys; vector<string> vals;
  for (auto const &m : this->transitionFunction) {
    keys = m.first; vals = m.second;
    for (string i : keys) {
      cout << i << ' ';
    }
    for (string i : vals) {
      cout << i << ' ';
    }
    cout << endl;
  }
}

int TuringMachine::run(string &input) {
  return -1;
}

int TuringMachine::singalStep() {
  return -1;
}