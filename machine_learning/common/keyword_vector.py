#!/usr/bin/python3

"""Some vector logic"""

import math
import os
import re
import sys

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
    vector = create_vector(dictionary)
    with open(filename, "r+") as file:
        line = file.readline()

        while not line == "":
            # DONE: replace all non-alphanumeric characters with space
            # words = line.split()
            words = re.sub('[^0-9a-zA-Z\-\_]', ' ', line).split() # pylint: disable=anomalous-backslash-in-string
            for word in words:
                if word in vector.keys():
                    vector[word] = vector[word] + 1
            line = file.readline()
    if normalise:
        normalise_vector(vector)
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
    new_vector = {}
    for key in vector.keys():
        new_vector[key] = vector[key]
    return new_vector


### Vector specific logic #################################################

def create_vector(dictionary):
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
    if not set(vec1.keys()) == set(vec2.keys()):
        print("Dictionaries don't have the same keys")
        return -1


    dist = 0
    for key in vec1:
        dist = dist + (vec1[key] - vec2[key]) ** 2

    dist = math.sqrt(dist)

    return dist


def normalise_vector(vec):
    """
    Take a given vector and normalise each entry to get a Euclidean
    distance of 1 between the zero vector and the vector itself.
    """
    sum_vals = 0
    for key in vec.keys():
        sum_vals = sum_vals + (vec[key] * vec[key])

    sum_vals = math.sqrt(sum_vals)

    for key in vec.keys():
        vec[key] = (vec[key] + 0.0) / sum_vals


def main():
    """
    Run some basic tests
    """
    if len(sys.argv) != 2:
        print("Usage: ")
        print(sys.argv[0] + " <dict_name>")
        sys.exit(1)

    dic = read_dictionary(sys.argv[1])

    testvector = {"hello":3, "bye":4}
    normalise_vector(testvector)
    print("normalised vector: " + str(testvector))

    vector1 = {"hello":3, "bye":4}
    normalise_vector(vector1)
    vector2 = {"hello":4, "bye":3}
    normalise_vector(vector2)
    print("distance same vector: " + str(vector_distance(vector1, vector1)))
    print("distance different vector: " + str(vector_distance(vector1, vector2)))
    print(vector1)
    print(vector2)

    print(count_occurances("../Singular/table.h", dic))

if __name__ == '__main__':
    main()
