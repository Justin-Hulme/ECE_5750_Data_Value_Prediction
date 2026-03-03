# ECE_5750_Data_Value_Prediction

## Building the Project

To build the executable:

```
make
```

This compiles the `cvp` program.

---

## Running All Traces and Grading

To run all traces, convert the results to CSV, and compare against a reference file:

```
make test TEST=<test_name> REF=<reference_csv>
```

Example:

```
make test TEST=allzero REF=results/last_value.csv
```

This will:

1. Build `cvp`
2. Run all traces in the `traces/` directory
3. Store output in a directory named `<test_name>`
4. Generate `results/<test_name>.csv`
5. Grade against the reference CSV

---

## Running a Single Trace

To run and grade only one trace:

```
make single TEST=<test_name> REF=<reference_csv> TRACE=<trace_file>
```

Example:

```
make single TEST=debug REF=results/last_value.csv TRACE=trace1
```

This will:

1. Build `cvp`
2. Run only the specified trace
3. Generate updated CSV results
4. Grade against the reference CSV

---

## Cleaning Build Files

To remove compiled objects and the executable:

```
make clean
```
