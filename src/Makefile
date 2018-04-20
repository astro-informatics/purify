
yaml-parser:
	g++ -c yaml-parser.cpp
test:
	g++ test.o ../src/yaml-parser.o -lyaml-cpp -lboost_program_options -o test

test_object:
	g++ -c test.cpp

objects:
	$(MAKE) test_object
	$(MAKE) yaml-parser

all:
	$(MAKE) clean
	$(MAKE) objects
	$(MAKE) test

clean:
	$(RM) test.o
	$(RM) yaml-parser.o
	$(RM) test
	$(RM) yaml-parser
