import sys
import csv

def read_csv(filepath):
    with open(filepath, newline="") as f:
        reader = csv.reader(f)
        header = next(reader)  # skip header
        rows = list(reader)
    return header, rows

def main():
    if len(sys.argv) != 3:
        print("Usage: python compare_csv.py file1.csv file2.csv")
        sys.exit(1)

    file1 = sys.argv[1]
    file2 = sys.argv[2]

    header1, rows1 = read_csv(file1)
    header2, rows2 = read_csv(file2)

    if len(rows1) != len(rows2):
        print("Error: CSV files must have the same number of data rows.")
        sys.exit(1)

    points = 0

    for row1, row2 in zip(rows1, rows2):
        try:
            val1 = float(row1[-1])
            val2 = float(row2[-1])
        except ValueError:
            print("Error: Rightmost column must contain numeric values.")
            sys.exit(1)

        if val2 <= val1:
            points += 2
            print(f"{row2[0]}\t\033[32mPASSED\033[0m: {float(row2[-1]):5.2f}% ≤ {float(row1[-1]):5.2f}%")
        else:
            # Print first and last column of the worse-performing row
            print(f"{row2[0]}\t\033[31mFAILED\033[0m: {float(row2[-1]):5.2f}% > {float(row1[-1]):5.2f}%")

    print(f"\nTotal points: {points}")

if __name__ == "__main__":
    main()