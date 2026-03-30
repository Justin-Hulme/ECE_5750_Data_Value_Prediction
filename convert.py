import re
from pathlib import Path
import csv
import sys

if len(sys.argv) != 2:
    print("Usage: python convert.py <results_dir>")
    sys.exit(1)

DIR = sys.argv[1]

header = ["trace", "prediction-eligible instructions", "correct predictions", "incorrect predictions", "percent predicted", "IPC"]

data = []

for path in Path(DIR).iterdir():
    if path.is_file():
        with open(path, "r") as file:
            coles_stupid_list = []
            
            coles_stupid_list.append(path.name)

            justins_stupid_list = [
                matches[0]
                for matches in (re.findall(r'= (\d+\.*\d*)', x) for x in file.readlines()[-6:-1])
                if matches
            ]

            justins_stupid_list.append(justins_stupid_list.pop(0))

            coles_stupid_list.extend(justins_stupid_list)

            coles_stupid_list.append(round(100*int(justins_stupid_list[1])/int(justins_stupid_list[0]),2))

            data.append(coles_stupid_list)

data.sort(key=lambda row: int(re.findall(r'\d+', row[0])[0]))

data.insert(0, header)

with open(f"results/{DIR}.csv", "w", newline="") as file:
    writer = csv.writer(file)
    writer.writerows(data)
            

