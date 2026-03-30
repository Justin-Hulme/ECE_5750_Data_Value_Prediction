CC = g++
OPT = -O3 
#OPT = -g
LIBS = -lz -lcvp -lz
FLAGS = -std=c++11 -L. $(LIBS) $(OPT) -no-pie

EXE ?= cvp

OBJ = mypredictor.o
DEPS = cvp.h mypredictor.h

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(FLAGS) -o $@ $^

%.o: %.cc $(DEPS)
	$(CC) $(FLAGS) -c -o $@ $<

.PHONY: clean test single

clean:
	rm -f *.o $(EXE)
	rm -rf results

# ------------------------
# FULL TEST (ALL TRACES)
# ------------------------
test: $(EXE)
	@if [ -z "$(TEST)" ] || [ -z "$(REF)" ] || [ -z "$(MODEL)" ]; then \
		echo "Usage: make test TEST=name REF=reference.csv MODEL=..."; \
		exit 1; \
	fi

	@echo "Running full test: $(TEST)"
	@mkdir -p $(TEST)

	@for f in traces/*; do \
		name=$$(basename $$f); \
		out="$(TEST)/$$name.txt"; \
		if [ ! -f "$$out" ]; then \
			echo "Running $$name"; \
			./$(EXE) -v $$f > "$$out" $(MODEL); \
		else \
			echo "Skipping $$name (already exists)"; \
		fi; \
	done

	@mkdir -p results
	@echo "Regenerating CSV..."
	@python3 convert.py $(TEST)

	@python3 grade.py results/$(TEST).csv $(REF)

# ------------------------
# SINGLE TRACE
# ------------------------
single: $(EXE)
	@if [ -z "$(TEST)" ] || [ -z "$(REF)" ] || [ -z "$(TRACE)" ]; then \
		echo "Usage: make single TEST=name REF=reference.csv TRACE=tracefile"; \
		exit 1; \
	fi

	@echo "Running single trace: $(TRACE)"
	@mkdir -p $(TEST)

	@TRACE_NAME=$$(basename $(TRACE)); \
	OUT="$(TEST)/$$TRACE_NAME.txt"; \
	if [ ! -f "$$OUT" ]; then \
		echo "Generating $$TRACE_NAME"; \
		./$(EXE) -v traces/$$TRACE_NAME > "$$OUT"; \
	else \
		echo "Skipping $$TRACE_NAME (already exists)"; \
	fi

	@mkdir -p results
	@echo "Regenerating CSV..."
	@python3 convert.py $(TEST)

	@python3 grade.py results/$(TEST).csv $(REF)