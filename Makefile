.DEFAULT_GOAL = all

PROGS=simpleteststring simpletestperson simpletestsafeunsigned simpletestunsigned btest perftest

CXX=g++
CXXFLAGS_TMP=-Wall -Wextra -Werror -std=c++17 -pedantic-errors
CXXFLAGS_OPT=$(CXXFLAGS_TMP) -O3
CXXFLAGS_DBG=$(CXXFLAGS_TMP) -Og -g

ifeq "$(PROD)" "true"
	CXXFLAGS=$(CXXFLAGS_OPT)
else
	CXXFLAGS=$(CXXFLAGS_DBG)
endif

ifeq "$(PHASE)" "2"
	PHASE=-DPH2
else
	PHASE=-DPH1
endif

simpletest: simpletestunsigned

simpleteststring:
	$(CXX) $(CXXFLAGS) $(PHASE) -DETYPE=std::string simpletest.cpp -o simpleteststring

simpletestperson:
	$(CXX) $(CXXFLAGS) $(PHASE) -DETYPE=Person simpletest.cpp -o simpletestperson

simpletestsafeunsigned:
	$(CXX) $(CXXFLAGS) $(PHASE) -DETYPE=SafeUnsigned simpletest.cpp -o simpletestsafeunsigned

simpletestunsigned:
	$(CXX) $(CXXFLAGS) $(PHASE) -DETYPE=unsigned simpletest.cpp -o simpletestunsigned

btest:
	$(CXX) $(CXXFLAGS) $(PHASE) -pthread btest.cpp -o btest

perftest:
	$(CXX) $(CXXFLAGS) -pthread performance_test.cpp -o perftest

all: $(PROGS)

clean:
	rm -Rf $(PROGS)

.PHONY: all clean

