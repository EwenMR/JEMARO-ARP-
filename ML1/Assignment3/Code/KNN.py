import os
import numpy as np
from tensorflow.keras.datasets import mnist
import matplotlib.pyplot as plt
import sys
from PIL import Image, ImageDraw, ImageFont

K = 3
NUM_TEST_SAMPLE = 1000

# Function to calculate Euclidean distance
def euclid_dist(a, b):
    squared_difference = (a - b)**2
    sum_squared_difference = np.sum(squared_difference, axis=1)
    distance = np.sqrt(sum_squared_difference)
    return distance

# Function to sort distances and return corresponding indices
def sort_distances(distances):
    indices = np.arange(len(distances))
    sorted_indices = np.argsort(distances)
    sorted_distances = distances[sorted_indices]
    sorted_indices = indices[sorted_indices]
    return sorted_distances, sorted_indices

# Function to get the most common value in a list
def get_most_common_value(lst):
    occurrences = {}
    for value in lst:
        if value in occurrences:
            occurrences[value] += 1
        else:
            occurrences[value] = 1
    most_common_value = max(occurrences, key=occurrences.get)
    return most_common_value

# Function to predict using k-Nearest Neighbors
def knn_predict(train_data, train_labels, test_data, k):
    if (train_data.shape[0]!=train_labels.shape[0]):
        print("Number of columns in training dataset does not equal the number of target.\n ")
        sys.exit()
    if (k<=0 or k>train_data.shape[0]):
        print("k has to be 0 < k < train data size\n")
        sys.exit()
    distances = euclid_dist(train_data, test_data)
    sorted_distances, sorted_indices = sort_distances(distances)
    
    nearest_indices = sorted_indices[:k]
    nearest_labels = train_labels[nearest_indices]
    
    predicted_label = get_most_common_value(nearest_labels)
    
    return predicted_label






def binary_knn(train_data, train_labels, test_data, test_label,target, k):
    TP=0
    TN=0
    FP=0
    FN=0
    prediction = knn_predict(train_data,train_labels,test_data,k)
    if prediction == target and target == test_label:
        TP=1
    elif prediction == target and target != test_label:
        FP=1
    elif prediction != target and target ==test_label:
        FN=1
    else:
        TN=1
    return TP,FP,FN,TN


# Main function
def main():
    correct=0
    k_values = [1,2,3,4,5,10,15,20,30,40,50]
    # Load MNIST data
    (train_images, train_labels), (test_images, test_labels) = mnist.load_data()

    # Reshape and normalize the images
    train_images = train_images.reshape((60000, 28 * 28)) / 255.0
    test_images = test_images.reshape((10000, 28 * 28)) / 255.0

    accuracies=[]
    error_rates=[]
    for k_value in k_values:
        print("k is " + str(k_value))
        total=[0,0,0,0]
        for target in range(10):
            # [True positive, False positive, False Negative, True Negative]
            confusion_matrix=[0,0,0,0]
            total_accuracy=0
            for j in range(NUM_TEST_SAMPLE):
                test_image = test_images[j].flatten()  # Flatten the 2D image
                confusion = binary_knn(train_images, train_labels, test_image,test_labels[j],target, k_value)
                for k in range(4):
                    confusion_matrix[k] += confusion[k]
                    total[k] += confusion[k]
            print(confusion_matrix)
            accuracy=(confusion_matrix[0]+confusion_matrix[3])*100/NUM_TEST_SAMPLE
            print("accuracy of " + str(target) + " is: "  + str(accuracy)+ "%")
        total_accuracy =  (total[0]+total[3])*100/(NUM_TEST_SAMPLE*10)
        accuracies.append(total_accuracy)
        error_rates.append(100-total_accuracy)
        print("total accuracy of when k is : " + str(k_value) + " is " + str(total_accuracy))

    bar_width = 2  # Adjust this value as needed

    # Plotting the bar chart with thicker bars
    plt.bar(k_values, error_rates, color='blue', width=bar_width)
    plt.xlabel('k Values')
    plt.ylabel('Error rate (%)')
    plt.title('Error rate vs k Values')
    plt.xticks(k_values)  # Show all k values on the x-axis
    plt.ylim(0,1)  # Set y-axis limits to percentage range
    plt.show()

    # for target in range(10):
    #     # [True positive, False positive, False Negative, True Negative]
    #     confusion_matrix=[0,0,0,0]
    #     for j in range(NUM_TEST_SAMPLE):
    #         test_image = test_images[j].flatten()  # Flatten the 2D image
    #         confusion = binary_knn(train_images, train_labels, test_image,test_labels[j],target, K)
    #         for k in range(4):
    #             confusion_matrix[k] += confusion[k]
    #     print(confusion_matrix)
    #     print("accuracy of " + str(target) + " is: "  + str((confusion_matrix[0]+confusion_matrix[3])*100/NUM_TEST_SAMPLE) + "%")



    # for i in range(NUM_TEST_SAMPLE):
    #     test_image = test_images[i].flatten()  # Flatten the 2D image
    #     predicted_label = knn_predict(train_images, train_labels, test_image, K)
    #     if predicted_label == test_labels[i]:
    #         correct+=1
    # print("Accuracy is: " + str(correct*100/NUM_TEST_SAMPLE) + "%")

    return 0

if __name__ == "__main__":
    main()
