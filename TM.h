#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

typedef struct {
  int index;
  char symbol;
} tapeCell;

class TuringMachine {
 public:
  TuringMachine(string &TMDef, bool verbose) {
    this->verbose = verbose;
    init_tm(TMDef);
  }

  int run(string &input);
  void displayTMDef();

 private:
  bool verbose;
  // TM symbols
  set<string> states;
  set<string> inputSymbols;
  set<string> tapeSymbols;
  string startState;
  string blankSymbol;
  set<string> finalStates;
  int numOfTapes;
  // TODO: transition function
  map<vector<string>, vector<string>>
      transitionFunction;  // state + input : next Symbol + direction + next
                           // state

  // TM status
  vector<vector<tapeCell>> tape;  // index + symbol
  vector<int> tapeHead;
  string curState;

  // Functions
  int init_tm(string &TMDef);
  int init_delta(string &deltaDef);

  int singalStep();

  // Verbose visualization
};


// ======= Helper Functions ========

set<string> split(string &str, char delimiter);