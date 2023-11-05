import tensorflow as tf
from tensorflow.keras.datasets import mnist
import sys

mnist = tf.keras.datasets.mnist
(train_X, train_y), (test_X, test_y) = mnist.load_data()
print(train_X)

def knn(train_data, train_target, test_data, k):
    if (train_data.shape[1]!=train_target.shape[1]):
        print("Number of columns in training dataset does not equal the number of target.\n ")
        sys.exit()
    if (k<=0 or k>train_data.shape[0]):
        print("k has to be 0 < k < train data size\n")
        sys.exit()

    