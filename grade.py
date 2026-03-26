import sys
import csv

def read_csv(filepath):
    with open(filepath, newline="") as f:
        reader = csv.reader(f)
        header = next(reader)
        rows = list(reader)
    return header, rows

def rows_to_dict(rows):
    return {row[0]: row for row in rows}

def main():
    if len(sys.argv) != 3:
        print("Usage: python compare_csv.py file1.csv file2.csv")
        sys.exit(1)

    file1 = sys.argv[1]
    file2 = sys.argv[2]

    _, rows1 = read_csv(file1)
    _, rows2 = read_csv(file2)

    dict1 = rows_to_dict(rows1)
    dict2 = rows_to_dict(rows2)

    common_keys = sorted(set(dict1.keys()) & set(dict2.keys()))
    missing_in_2 = set(dict1.keys()) - set(dict2.keys())
    missing_in_1 = set(dict2.keys()) - set(dict1.keys())

    if missing_in_1:
        print(f"Warning: {len(missing_in_1)} entries missing in {file1}")
    if missing_in_2:
        print(f"Warning: {len(missing_in_2)} entries missing in {file2}")

    points = 0

    for key in common_keys:
        row1 = dict1[key]
        row2 = dict2[key]

        try:
            val1 = float(row1[-1])
            val2 = float(row2[-1])
        except ValueError:
            print(f"{key}\tError: non-numeric value")
            continue

        if val2 <= val1:
            points += 2
            print(f"{key}\t\033[32mPASSED\033[0m: {val2:5.2f}% ≤ {val1:5.2f}%")
        else:
            print(f"{key}\t\033[31mFAILED\033[0m: {val2:5.2f}% > {val1:5.2f}%")

    print(f"\nCompared {len(common_keys)} rows")
    print(f"Total points: {points}")

if __name__ == "__main__":
    main()