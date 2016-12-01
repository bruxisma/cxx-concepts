run: all
	@./presentation

all: presentation

presentation: presentation.o
	clang++ -std=c++14 -o $@ $< 

%.o : %.cxx
	clang++ -std=c++14 -o $@ -c $<

presentation.o: presentation.cxx

clean:
	$(RM) presentation *.o

.PHONY: clean run
