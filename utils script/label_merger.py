import csv

# Dictionary mapping letters to numbers
correspondence = {
    'A': 0, 'B': 1, 'C': 2, 'D': 3, 'E': 4,
    'F': 5, 'G': 6, 'H': 7, 'I': 8, 'J': 9,
    'K': 10, 'L': 11, 'M': 12, 'N': 13, 'O': 14,
    'P': 15, 'Q': 16, 'R': 17, 'S': 18, 'T': 19,
    'U': 20, 'V': 21, 'W': 22, 'X': 23, 'Y': 24, 'Z': 25
}

# List of labels to keep
letters_to_keep = ['C', 'H', 'L', 'Y']
labels_to_keep = [correspondence[letter] for letter in letters_to_keep]

# Replacement label for those not in the list
replacement_letter = 'C'
replacement_label = correspondence[replacement_letter]

# Function to replace labels not in the specified list
def replace_labels(label):
    return label if label in labels_to_keep else replacement_label

# Input and output CSV file names
input_file = 'data/sign_mnist_train_2.csv'
output_file = 'data/sign_mnist_train_3.csv'

with open(input_file, 'r', newline='') as file_in, open(output_file, 'w', newline='') as file_out:
    csv_reader = csv.reader(file_in)
    csv_writer = csv.writer(file_out)

    # Read the first line (headers)
    headers = next(csv_reader)
    csv_writer.writerow(headers)

    # Iterate over the remaining lines
    for line in csv_reader:
        # Retrieve the label from the first column
        label = int(line[0])

	# Replace the label if necessary
    	new_label = replace_labels(label)

    	# Write the line with the new label to the output file
    	csv_writer.writerow([new_label] + line[1:])
