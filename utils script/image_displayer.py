import csv
import numpy as np
import matplotlib.pyplot as plt

# Function to convert a list of values into a 28x28 image
def convert_to_image(data):
    # Convert the list into a numpy array of dimensions 28x28
    image_array = np.array(data, dtype=np.uint8).reshape(28, 28)
    return image_array

# Name of the CSV file containing the pixel values
csv_file = "img_compar.csv"

# Lists to store the pixel values of the two images
images = []

# Read the CSV file and store the pixel values in the list "images"
with open(csv_file, newline='') as csvfile:
    csvreader = csv.reader(csvfile)
    header = next(csvreader)  # Skip the header of the CSV file
    for row in csvreader:
        images.append([int(pixel) for pixel in row])

# Convert the lists of values into 28x28 images
image_1 = convert_to_image(images[0])
image_2 = convert_to_image(images[1])

# Display the two images
plt.figure(figsize=(5, 5))

# First image
plt.subplot(1, 2, 1)
plt.imshow(image_1, cmap='gray')
plt.title('Image 1')
plt.axis('off')

# Second image
plt.subplot(1, 2, 2)
plt.imshow(image_2, cmap='gray')
plt.title('Image 2')
plt.axis('off')

plt.tight_layout()
plt.show()
