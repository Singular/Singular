#!/usr/bin/python3

"""Some vector logic"""

import math
import os
import re
import sys
import numpy as np

### Read from file ########################################################

def read_dictionary(filename):
    """
    Read a dictionary saved as a textfile
    """
    if not os.path.isfile(filename):
        print("Please provide a valid input file as argument")
        return []

    dictionary = []

    with open(filename, "r") as file:
        line = file.readline()

        while not line == "":
            dictionary.append(line.strip())
            line = file.readline()
    return dictionary


def count_occurances(filename, dictionary, normalise=True):
    """
    Create a vector from a dictionary and populate the counts according to
    a specified file
    """
    if not os.path.isfile(filename):
        print("Please provide a valid input file as argument")
        return {}
    if dictionary == []:
        print("Please provide a valid dictionary as argument")
        return {}
    if dictionary is None:
        print("Please provide a valid dictionary as argument")
        return {}
    vector = create_vector_dictionary(dictionary)
    with open(filename, "r+") as file:
        line = file.readline()

        while not line == "":
            words = re.sub('[^0-9a-zA-Z\-\_]', ' ', line).split() # pylint: disable=anomalous-backslash-in-string
            for word in words:
                if word in vector.keys():
                    vector[word] = vector[word] + 1
            line = file.readline()
    vector = np.array(list(vector.values()))
    if normalise:
        vector = normalise_vector(vector)
    return vector 


### Copying ###############################################################

def copy_dictionary(dictionary):
    """
    Return an identical copy of a dictionary
    """
    new_dic = []
    for word in dictionary:
        new_dic.append(word)
    return new_dic


def copy_vector(vector):
    """
    Return an identical copy of a vector
    """
    return np.copy(np.array(vector))


### Vector specific logic #################################################

def create_vector_dictionary(dictionary):
    """
    Create a zero vector for a given dictionary
    """
    vector = {}
    for word in dictionary:
        vector[word] = 0
    return vector


def vector_distance(vec1, vec2):
    """
    Calculate the Euclidean distance between two vectors.
    """
    if not len(vec1) == len(vec2):
        print("Vectors don't have the same sizes")
        return -1

    diff_vec = vec1 - vec2
    dist = np.linalg.norm(diff_vec)

    return dist


def normalise_vector(vec):
    """
    Take a given vector and normalise each entry to get a Euclidean
    distance of 1 between the zero vector and the vector itself.
    """
    vec = vec / np.linalg.norm(vec)
    return vec


def main():
    """
    Run some basic tests
    """
    if len(sys.argv) != 2:
        print("Usage: ")
        print(sys.argv[0] + " <dict_name>")
        sys.exit(1)

    testvector = np.array([3,4])
    normalise_vector(testvector)
    print("normalised vector: " + str(testvector))

    vector1 = np.array([3,4])
    vector1 = normalise_vector(vector1)
    vector2 = np.array([4,3])
    normalise_vector(vector2)
    vector2 = normalise_vector(vector2)
    print("distance same vector: " + str(vector_distance(vector1, vector1)))
    print("distance different vector: " + str(vector_distance(vector1, vector2)))
    print(vector1)
    print(vector2)

    dic = read_dictionary(sys.argv[1])
    print(count_occurances("../Singular/table.h", dic))

if __name__ == '__main__':
    main()
