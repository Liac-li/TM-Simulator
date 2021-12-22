cc = g++
prom = turing
src = main.cpp TM.cpp
CFLAG = -std=c++11

$(prom): $(src)
	$(cc) $(CFLAG) -o $(prom) $(src)
	
clean:
	rm turing
	
run:
	./turing -v ../palindrome_detector_2tapes.tm $(INPUT)