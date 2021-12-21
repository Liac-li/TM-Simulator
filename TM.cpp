#include <string>

using namespace std;


class TuringMachine {
    public:
        TuringMachine(string &TMDef, bool verbose) {
            this->verbose = verbose;
            init_tm(TMDef);
        }

    void run(); 
    
    private:
        bool verbose;
        // TM symbols 

        // Functions 
        int init_tm(string &TMDef);
        
        
        // Verbose visualization
};

