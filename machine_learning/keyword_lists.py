#!/usr/bin/python3

import math
import os
import re
import sys

### Read from file ########################################################

def read_dictionary(filename):
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


def count_occurances(filename, dictionary):
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
            words = re.sub('[^0-9a-zA-Z\-\_]', ' ', line).split()
            for word in words:
                if word in vector.keys():
                    vector[word] = vector[word] + 1
            line = file.readline()
    return vector


### Copying ###############################################################

def copy_dictionary(dictionary):
    new_dic = []
    for word in dictionary:
        new_dic.append(word)
    return new_dic


def copy_vector(vector):
    new_vector = {}
    for key in vector.keys():
        new_vector[key] = vector[key]
    return new_vector


### Vector specific logic #################################################

def create_vector(dictionary):
    vector={}
    for word in dictionary:
        vector[word] = 0
    return vector


def vector_distance(vec1, vec2):
    if not set(vec1.keys()) == set(vec2.keys()):
        print("Dictionaries don't have the same keys")
        return -1

    nvec1 = copy_vector(vec1)
    nvec2 = copy_vector(vec2)
    normalise_vector(nvec1)
    normalise_vector(nvec2)

    dist = 0
    for key in nvec1.keys():
        dist = dist + (nvec1[key] - nvec2[key]) ** 2

    dist = math.sqrt(dist)
    
    return dist


def normalise_vector(vec):
    sum = 0
    for key in vec.keys():
        sum = sum + (vec[key] * vec[key])
    
    sum = math.sqrt(sum)

    for key in vec.keys():
        vec[key] = (vec[key] + 0.0) / sum


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: ")
        print(sys.argv[0] + " <dict_name>")
        sys.exit(1)

    dic = read_dictionary(sys.argv[1])

    vector = {"hello":3, "bye":4}
    normalise_vector(vector)
    print("normalised vector: " + str(vector))

    vector1 = {"hello":3, "bye":4}
    vector2 = {"hello":4, "bye":3}
    print("distance same vector: " + str(vector_distance(vector1, vector1)))
    print("distance different vector: " + str(vector_distance(vector1, vector2)))
    print(vector1)
    print(vector2)

    print(count_occurances("Singular/table.h", dic))
