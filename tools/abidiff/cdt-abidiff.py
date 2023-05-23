import json
import sys

def compare_abi_files(file1, file2):

    # check if the files are exist and try to read data from them
    for file in [file1, file2]:
        if not os.path.isfile(file):
            print(f"Error: File '{file}' does not exist.")
            return
    try:
        with open(file1, 'r') as f1, open(file2, 'r') as f2:
            data1, data2 = json.load(f1), json.load(f2)
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in file '{e.filename}' at position {e.pos}.")
        return
    for data, file in [(data1, file1), (data2, file2)]:
        if not isinstance(data, dict):
            print(f"Error: File '{file}' does not contain a JSON object.")
            return

    diff = {}

    # add to the diff dictionary the different pairs of values for a key
    # if a key is exists in the only file add None instead of the paired data for the second file.
    for key in data1:
        if key not in data2:
            diff[key] = {file1: data1[key], file2: None}
        elif data1[key] != data2[key]:
            diff[key] = {file1: data1[key], file2: data2[key]}
    for key in data2:
        if key not in data1:
            diff[key] = {file1: None, file2: data2[key]}

    if not diff:
        print("Files are identical.")
    else:
        print("Differences found:")
        print(json.dumps(diff, indent=4))


if len(sys.argv) != 3:
    print("Usage: python3" +  sys.argv[0] + " <file1.abi> <file2.abi>")
else:
    file1, file2 = sys.argv[1], sys.argv[2]
    compare_abi_files(file1, file2)
