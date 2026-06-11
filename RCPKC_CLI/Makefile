CXX ?= g++
CXXFLAGS ?= -O2 -std=c++17 -Wall -Wextra
LDLIBS ?= -lntl -lgmp -lm

all: rcpkc_cli

rcpkc_cli: rcpkc_cli.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f rcpkc_cli public.key private.key cipher.txt ResultsFile_cli.csv
