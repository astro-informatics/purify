yaml-parser:
	g++ -c yaml-parser.cpp
parser-test:
	g++ parser-test.o ../src/yaml-parser.o -lyaml-cpp -lboost_program_options -o parser-test

parser-test_object:
	g++ -c parser-test.cpp

objects:
	$(MAKE) parser-test_object
	$(MAKE) yaml-parser

all:
	$(MAKE) clean
	$(MAKE) objects
	$(MAKE) parser-test

clean:
	$(RM) parser-test.o
	$(RM) yaml-parser.o
	$(RM) parser-test
	$(RM) yaml-parser
