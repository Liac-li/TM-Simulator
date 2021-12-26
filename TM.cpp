#include "TM.h"

#include <algorithm>
#include <fstream>
#include <iostream>

using namespace std;

/*
 * FLA21Fall NJUCS
 * LYT - lyt-42@outlook.com
 * Dec 22, 2021
 *
 * Simulate Turing Machine
 * 1. [x] Read input from file and generate TM
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

vector<int> getBound(const vector<char> tape, const int head,
                     const char blank) {
  int lBound = 0;
  int rBound = tape.size() - 1;

  for (; lBound < tape.size() && tape[lBound] == blank && lBound < head;
       lBound++)
    ;
  for (; rBound >= 0 && tape[rBound] == blank && rBound > head; rBound--)
    ;

  vector<int> res = {lBound, rBound};
  return res;
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

          set<char> res;
          for (string i : tmp_set) {
            res.insert(i[0]);
          }

          this->inputSymbols = res;
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
          // string blank(tmp_line, 5, 1);
          this->blankSymbol = tmp_line[5];
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

  for (int i = 0; i < this->numOfTapes; i++) {
    vector<char> tmp_tape;
    tmp_tape.push_back(this->blankSymbol);

    this->tape.push_back(tmp_tape);
    this->tapeHead.push_back(0);
    this->initHead.push_back(0);
  }
  this->curState = this->startState;
  this->globalStep = 0;

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
  vector<set<string>> items = {this->states, this->tapeSymbols,
                               this->finalStates};
  vector<string> names = {"states", "tapeSymbol", "finalStates"};
  for (int idx = 0; idx < 3; idx++) {
    cout << names[idx] << " : " << endl;
    for (string item : items[idx]) {
      cout << item << ' ';
    }
    cout << endl;
  }

  cout << "Input Symbols: ";
  for (char ch : this->inputSymbols) {
    cout << ch << ' ';
  }
  cout << endl;

  cout << "startState: " << this->startState << endl;
  cout << "blankSymbol: " << this->blankSymbol << endl;
  cout << "tape num: " << this->numOfTapes << endl;

  vector<string> keys;
  vector<string> vals;
  for (auto const &m : this->transitionFunction) {
    keys = m.first;
    vals = m.second;
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
  /*
   * Run TM on given input string
   */

  if (checkInput(input) != 0) {
    exit(-1);
  }
  init_tape(input);

  if (this->verbose) {
    cout << "Input: " << input << endl;
    cout << "==================== RUN ====================" << endl;
    stepDisplay();
  }

  int ret_code = singalStep();
  while (ret_code == 0) {
    ret_code = singalStep();
  }

  switch (ret_code) {
    case 1: {  // halt
      if (this->verbose) cout << "[Halt] Result: ";
    } break;
    case 2: {  // accept
      if (this->verbose) cout << "[Accepted] Result: ";
    } break;
    default:
      if (this->verbose) cerr << "Something Error in Running" << endl;
      exit(-1);
  }

  // display the item on tape[0]
  vector<int> tmp =
      getBound(this->tape[0], this->tapeHead[0], this->blankSymbol);
  int l = tmp[0];
  int r = tmp[1];
  for (int i = l; i <= r; i++) {
    cout << this->tape[0][i];
  }
  cout << endl;

  if (this->verbose) {
    cout << "==================== END ====================" << endl;
  }

  return -1;
}

int TuringMachine::checkInput(string &input) {
  /*
   * check input illegality
   */
  for (auto i = 0; i < input.length(); i++) {
    if (this->inputSymbols.find(input[i]) ==
        this->inputSymbols.end()) {  // not in symbol set
      if (this->verbose) {
        cerr << "Input: " << input << endl;
        cerr << "==================== ERR ====================" << endl;
        cerr << "error: '" << input[i]
             << "' was not declared in the set of input symbols" << endl;
        cerr << "Input: " << input << endl;
        string pnt(7 + i, ' ');
        pnt += "^";
        cerr << pnt << endl;
        cerr << "==================== END ====================" << endl;
      } else {
        cerr << "Input: " << input << endl;
        cerr << input[i] << " was illegal input symbol" << endl;
      }
      return -1;
    }
  }

  return 0;
}

int TuringMachine::init_tape(string &input) {
  this->tape[0].pop_back();
  for (char ch : input) {
    this->tape[0].push_back(ch);
  }
  return 0;
}

int TuringMachine::singalStep() {
  /*
   * step on in TM
   * return: 1 => halt, 2 => accept, 0 => continue, -1 => error
   */

  this->globalStep++;
  // if (this->globalStep > 12) {
  // return -1;
  // }

  // get keys of transition
  string heads;
  for (int i = 0; i < this->numOfTapes; i++) {
    heads += this->tape[i][this->tapeHead[i]];
  }
  vector<string> curkeys;
  curkeys.push_back(this->curState);
  curkeys.push_back(heads);
  // if (this->transitionFunction.find(curkeys) ==
  //     this->transitionFunction.end()) {
  //   // no transition exists -> halt
  //   if (this->verbose) {
  //     stepDisplay();
  //   }
  //   return 1;
  // }
  // vector<string> nextEnv = this->transitionFunction[curkeys];
  vector<string> nextEnv = getNextEnv(curkeys);
  if (nextEnv.size() < 1) {  // halt, no next state
    if (this->verbose) {
      stepDisplay();
    }
    return 1;
  }

  string nextTapeVals = nextEnv[0];
  string nextDircts = nextEnv[1];
  string nextState = nextEnv[2];

  // one step ahead on TM
  for (int tapeId = 0; tapeId < this->numOfTapes; tapeId++) {
    if (nextTapeVals[tapeId] != '*') {
      this->tape[tapeId][this->tapeHead[tapeId]] = nextTapeVals[tapeId];
    }

    if (nextDircts[tapeId] == 'r') {
      this->tapeHead[tapeId] += 1;
      if (this->tapeHead[tapeId] >= this->tape[tapeId].size()) {
        this->tape[tapeId].push_back(this->blankSymbol);
      }

    } else if (nextDircts[tapeId] == 'l') {
      // may over range with push_back, need to remain the index of tape
      this->tapeHead[tapeId] -= 1;
      if (this->tapeHead[tapeId] < 0) {
        this->tapeHead[tapeId] += 1;
        this->tape[tapeId].insert(this->tape[tapeId].begin(),
                                  this->blankSymbol);
        this->initHead[tapeId] += 1;  // for tape index
      }
    } else {  // stay
      continue;
    }
  }
  this->curState = nextState;

  if (this->finalStates.find(this->curState) != this->finalStates.end()) {
    if (this->verbose) {
      stepDisplay();
    }
    return 2;
  }

  if (this->verbose) {
    stepDisplay();
  }
  return 0;

  // return -1;
}

void TuringMachine::stepDisplay() {
  /*
   *  Step    : 0
   *  Index0  : 0 1 2 3 4 5 6
   *  Tape0   : 1 0 0 1 0 0 1
   *  Head0   : ^
   *  Index1  : 0
   *  Tape1   : _
   *  Head1   : ^
   *  State   : 0
   *  ---------------------------------------------
   */
  cout << "Step  \t: " << this->globalStep << endl;
  for (int tapeId = 0; tapeId < this->numOfTapes; tapeId++) {
    auto tmp =
        getBound(this->tape[tapeId], this->tapeHead[tapeId], this->blankSymbol);
    int lBound = tmp[0];
    int rBound = tmp[1];

    cout << "Index" << tapeId << " \t: ";
    vector<int> alignLength;
    for (int i = lBound; i <= rBound; i++) {
      int index = i - this->initHead[tapeId];
      index = index >= 0 ? index : -index;  // abs
      cout << index << " ";
      alignLength.push_back(to_string(index).length());
    }
    cout << endl;

    cout << "Tape" << tapeId << " \t: ";
    for (int i = lBound; i <= rBound; i++) {
      string blanks = string(alignLength[i - lBound], ' ');
      cout << this->tape[tapeId][i] << blanks;
    }
    cout << endl;

    cout << "Head" << tapeId << " \t: ";
    for (int i = lBound; i < this->tapeHead[tapeId] - lBound; i++) {
      string blanks = string(alignLength[i], ' ');
      cout << " " << blanks;
    }
    cout << "^" << endl;
  }

  cout << "State \t: " << this->curState << endl;
  cout << "---------------------------------------------" << endl;
}

vector<string> TuringMachine::getNextEnv(const vector<string> keys) {
  bool isMatch = false;
  // vector<bool> matchingRes;
  // vector<vector<string>> matchingKeys;
  map<vector<string>, int> mathchingKeys2priority;

  for (auto item : this->transitionFunction) {
    if (item.first[0] == keys[0]) {  // same state
      // cerr << item.first[0] << keys[0] << item.first[1] << keys[1] << endl;

      for (int i = 0; i < this->numOfTapes; i++) {
        if (item.first[1][i] == keys[1][i] || item.first[1][i] == '*') {
          isMatch = true;
        } else {
          isMatch = false;
          break;
        }
      }
      if (isMatch) {
        mathchingKeys2priority[item.first] =
            count(item.first[1].begin(), item.first[1].end(), '*');
      }
    }
    isMatch = false;
  }

  int min = this->numOfTapes + 1;
  vector<string> finalKey;
  for (auto item : mathchingKeys2priority) {
    if (item.second < min) {
      min = item.second;
      finalKey = item.first;
    }
  }
  if (finalKey.size() > 0) {
    return this->transitionFunction[finalKey];
  }
  return finalKey;
}