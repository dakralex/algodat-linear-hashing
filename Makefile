PROGS=simpletest btest

CXX=g++
CXXFLAGS=-Wall -Wextra -Werror -std=c++17 -pedantic-errors
CXXFLAGS_OPT=$(CXXFLAGS) -O3
CXXFLAGS_DBG=$(CXXFLAGS) -Og -g

simpletest1string:
	$(CXX) $(CXXFLAGS_OPT) -DPH1 -DETYPE=std::string simpletest.cpp -o simpletest

simpletest1person:
	$(CXX) $(CXXFLAGS_OPT) -DPH1 -DETYPE=Person simpletest.cpp -o simpletest

simpletest1safeunsigned:
	$(CXX) $(CXXFLAGS_OPT) -DPH1 -DETYPE=SafeUnsigned simpletest.cpp -o simpletest

simpletest1unsigned:
	$(CXX) $(CXXFLAGS_OPT) -DPH1 -DETYPE=unsigned simpletest.cpp -o simpletest

simpletest2string:
	$(CXX) $(CXXFLAGS_OPT) -DPH2 -DETYPE=std::string simpletest.cpp -o simpletest

simpletest2person:
	$(CXX) $(CXXFLAGS_OPT) -DPH2 -DETYPE=Person simpletest.cpp -o simpletest

simpletest2safeunsigned:
	$(CXX) $(CXXFLAGS_OPT) -DPH2 -DETYPE=SafeUnsigned simpletest.cpp -o simpletest

simpletest2unsigned:
	$(CXX) $(CXXFLAGS_OPT) -DPH2 -DETYPE=unsigned simpletest.cpp -o simpletest

simpletest: simpletest1string

btest1:
	$(CXX) $(CXXFLAGS_DBG) -DPH1 -pthread btest.cpp -o btest

btest2:
	$(CXX) $(CXXFLAGS_DBG) -DPH1 -pthread btest.cpp -o btest

btest: btest1

clean:
	rm -Rf $(PROGS)

.PHONY: all clean

