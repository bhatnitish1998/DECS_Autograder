import os
import glob


def parse_file(file_path):
    with open(file_path, "r") as file:
        line = file.readline().strip()
        values = [float(val) for val in line.split(",")]
    return values


def add_columns(files):
    # Initialize a dictionary to store column sums
    column_sums = {}

    for file_path in files:
        # Parse the values from the current file
        values = parse_file(file_path)

        # Add the values to the corresponding columns
        for i, val in enumerate(values):
            if i not in column_sums:
                column_sums[i] = val
            else:
                column_sums[i] += val

    return column_sums


if __name__ == "__main__":
    # Specify the pattern using a regular expression
    file_pattern = "client*.txt"

    # Use glob to find files matching the pattern
    file_paths = glob.glob(file_pattern)

    # Add the corresponding columns across different files
    result = add_columns(file_paths)

    # Print the results
    for column, sum_value in result.items():
        print(f"Column {column + 1}: {sum_value}")
