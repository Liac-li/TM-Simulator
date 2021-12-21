cc = g++
prom = turing
src = main.cpp TM.cpp

$(prom): $(src)
	$(cc) -o $(prom) $(src)
	
clean:
	rm turing
	
run:
	./turing -v ../palindrome_detector_2tapes.tm $(INPUT)