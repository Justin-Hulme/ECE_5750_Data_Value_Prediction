CC = g++
OPT = -O3 
#OPT = -g
LIBS = -lz -lcvp -lz
FLAGS = -std=c++11 -L. $(LIBS) $(OPT) -no-pie

OBJ = mypredictor.o
DEPS = cvp.h mypredictor.h

all: cvp

cvp: $(OBJ)
	$(CC) $(FLAGS) -o $@ $^

%.o: %.cc $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $<

.PHONY: clean test single

clean:
	rm -f *.o cvp

test: cvp
	@if [ -z "$(TEST)" ] || [ -z "$(REF)" ]; then \
		echo "Usage: make test TEST=name REF=reference.csv"; \
		exit 1; \
	fi
	@echo "Running full test: $(TEST)"
	@if [ -d "$(TEST)" ]; then \
		echo "Test directory exists, skipping trace run + conversion."; \
	else \
		./runalltraces.sh $(TEST); \
		mkdir -p results; \
		python3 convert.py $(TEST); \
	fi
	@mkdir -p results
	@python3 grade.py results/$(TEST).csv $(REF)

single: cvp
	@if [ -z "$(TEST)" ] || [ -z "$(REF)" ] || [ -z "$(TRACE)" ]; then \
		echo "Usage: make single TEST=name REF=reference.csv TRACE=tracefile"; \
		exit 1; \
	fi
	@echo "Running single trace: $(TRACE)"
	@if [ -d "$(TEST)" ]; then \
		echo "Test directory exists, skipping trace run."; \
	else \
		mkdir -p $(TEST); \
		./cvp -v traces/$(TRACE) > $(TEST)/$(TRACE).txt; \
		mkdir -p results; \
		python3 convert.py $(TEST); \
	fi
	@mkdir -p results
	@python3 grade.py results/$(TEST).csv $(REF)