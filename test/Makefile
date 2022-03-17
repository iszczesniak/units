TESTS = cunits sunits

#CXXFLAGS = -g -Wno-deprecated
CXXFLAGS = -O3 -Wno-deprecated

CXXFLAGS := -std=c++20 -I ../

# Run the tests.
run: $(TESTS)
	@for i in $(TESTS); do echo "Running" $$i; ./$$i; done

count:
	wc -l *.hpp *.cc

clean:
	rm -rf *~
	rm -rf *.o
	rm -rf $(TESTS)

depend:
	g++ -MM -I../ *.cc > dependencies

include dependencies
