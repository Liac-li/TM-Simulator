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
  set<char> inputSymbols;
  set<string> tapeSymbols;
  string startState;
  char blankSymbol;
  set<string> finalStates;
  int numOfTapes;
  // TODO: transition function
  map<vector<string>, vector<string>>
      transitionFunction;  // state + input : next Symbol + direction + next
                           // state

  // TM status
  vector<vector<char>> tape;  // index + symbol
  vector<int> tapeHead;
  string curState;
  vector<int> initHead; // use to get index of tape

  // Functions
  int init_tm(string &TMDef);
  int init_delta(string &deltaDef);

  int globalStep = 0;
  int singalStep();
  int checkInput(string &input);
  int init_tape(string &input);
  vector<string> getNextEnv(const vector<string> keys);

  // Verbose visualization
  void stepDisplay();
};


// ======= Helper Functions ========

set<string> split(string &str, char delimiter);