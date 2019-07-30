#!/usr/bin/python3

"""Some vector logic"""

import os
import re
import numpy as np

from common.constants import KEYWORDS_FILE

### Read from file ########################################################

def read_dictionary(filename=KEYWORDS_FILE):
    """
    Read a dictionary saved as a textfile
    """
    if not os.path.isfile(filename):
        print("Please provide a valid input file as argument to read "
              "dictionary")
        raise FileNotFoundError

    dictionary = []

    with open(filename, "r") as file:
        line = file.readline()

        while not line == "":
            dictionary.append(line.strip())
            line = file.readline()
    return np.array(dictionary)


def count_occurances(filename, dictionary, normalise=True):
    """
    Create a vector from a dictionary and populate the counts according to
    a specified file
    """
    if not os.path.isfile(filename):
        print("Please provide a valid input file as argument")
        raise FileNotFoundError
    assert dictionary is not None, \
            "Please provide a valid dictionary as argument"
    assert not dictionary.size == 0, \
            "Please provide a valid dictionary as argument"

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
    return np.copy(np.array(dictionary))


def copy_vector(vector):
    """
    Return an identical copy of a vector
    """
    return np.copy(np.array(vector))


### Vector specific logic #################################################

def create_vector_dictionary(dictionary):
    """
    Create a zero lookup dictionary for a given dictionary
    """
    assert not dictionary is None, "Please give a dictionary"
    assert not np.array(dictionary).size == 0, "Please give a dictionary"
    vector = {}
    for word in dictionary:
        vector[word] = 0
    return vector


def vector_distance(vec1, vec2):
    """
    Calculate the Euclidean distance between two vectors.
    """
    assert len(vec1) == len(vec2), \
            "Vectors don't have the same sizes"

    dist = np.linalg.norm(vec1 - vec2)

    return dist


def normalise_vector(vec):
    """
    Take a given vector and normalise each entry to get a Euclidean
    distance of 1 between the zero vector and the vector itself.
    """

    if vec is None:
        print("Warning, None is not a valid vector")
        print("Returning empty vector by default")
        return np.array([])

    if not isinstance(vec, np.ndarray):
        print("Warning, vector should be a numpy array")

    if np.array(vec).size == 0:
        print("Warning, vector being normalised is empty")

    norm = np.linalg.norm(vec)
    if not norm == 0:
        vec = vec / norm
    return vec

def test_read_dictionary():
    """
    Create test for read_dictionary function
    """
    print("\033[1;32;40mTesting read_dictionary function:\033[1;37;40m")

    print("Non-existant file")
    correct = False
    try:
        read_dictionary("asdfasdf")
    except FileNotFoundError:
        print("correctly caught non-existant file")
        print("\033[1;32;40mpass\033[1;37;40m")
        correct = True
    if not correct:
        print("\033[1;31;40mfail\033[1;37;40m")

    print("Reading default file")
    correct = True
    try:
        read_dictionary()
    except FileNotFoundError:
        print("Default file for dictionary missing")
        print("\033[1;31;40mfail\033[1;37;40m")
        correct = False
    if correct:
        print("\033[1;32;40mpass\033[1;37;40m")
    print()
    print()

def test_count_occurances():
    """
    Create test for count_occurances function
    """
    print("\033[1;32;40mTesting count_occurances function:\033[1;37;40m")
    dic = read_dictionary()
    correct = False
    try:
        vec = count_occurances("asdfasdf", dic)
    except FileNotFoundError:
        correct = True
        print("Correctly raised FileNotFoundError")
        print("\033[1;32;40mpass\033[1;37;40m")
    if not correct:
        print("\033[1;31;40mfail\033[1;37;40m")

    print("Count occurances with None dictionary:")
    correct = False
    try:
        count_occurances("../Singular/table.h", None)
    except AssertionError:
        print("Correctly caught AssertionError")
        print("\033[1;32;40mpass\033[1;37;40m")
        correct = True
    if not correct:
        print("\033[1;31;40mfail\033[1;37;40m")


    print("Count occurances with empty dictionary:")
    correct = False
    try:
        count_occurances("../Singular/table.h", np.array([]))
    except AssertionError:
        print("Correctly caught AssertionError")
        print("\033[1;32;40mpass\033[1;37;40m")
        correct = True
    if not correct:
        print("\033[1;31;40mfail\033[1;37;40m")


    print("vector of ../Singular/table.h")
    vec = count_occurances("../Singular/table.h", dic)
    print(vec)
    print()
    print()

def test_create_vector_dictionary():
    """
    Create test for create_vector_dictionary function
    """
    print("\033[1;32;40mTesting create_vector_dictionary " \
            "function:\033[1;37;40m")
    read_dictionary()

    print("Create Vector Dictionary with None as dictionary:")
    correct = False
    try:
        create_vector_dictionary(None)
    except AssertionError:
        correct = True
        print("\033[1;32;40mpass\033[1;37;40m")
    if not correct:
        print("\033[1;31;40mfail\033[1;37;40m")

    print("Create Vector Dictionary with empty dictionary:")
    correct = False
    try:
        create_vector_dictionary(np.array([]))
    except AssertionError:
        correct = True
        print("\033[1;32;40mpass\033[1;37;40m")
    if not correct:
        print("\033[1;31;40mfail\033[1;37;40m")

    print()
    print()

def test_vector_distance():
    """
    Create test for vector_distance function
    """
    print("\033[1;32;40mTesting vector_distance function:\033[1;37;40m")

    vector1 = np.array([3, 4])
    vector1 = normalise_vector(vector1)
    vector2 = np.array([4, 3])
    vector2 = normalise_vector(vector2)

    print("Distance of vectors of different dimensions:")
    correct = False
    try:
        vector_distance(np.array([1, 2, 3]), vector1)
    except AssertionError:
        correct = True
        print("\033[1;32;40mpass\033[1;37;40m")
    if not correct:
        print("\033[1;31;40mfail\033[1;37;40m")


    print("Distance same vector: " + str(vector_distance(vector1, vector1)))
    assert vector_distance(vector1, vector1) == 0, \
            "distance to same vectorshould be 0"
    print("\033[1;32;40mpass\033[1;37;40m")

    print("Distance different vector: " + str(vector_distance(vector1, vector2)))
    assert vector_distance(vector1, vector2) > 0, \
            "Distance between nonequal vectors should be strictly positive"
    print("\033[1;32;40mpass\033[1;37;40m")
    print()
    print()

def test_normalise_vector():
    """
    Create test for normalise_vector function
    """
    print("\033[1;32;40mTesting normalise_vector function:\033[1;37;40m")
    testvector = np.array([3, 4])
    testvector = normalise_vector(testvector)
    assert np.linalg.norm(testvector) == 1, \
            "Normalised vector should have norm of 1"
    print("\033[1;32;40mpass\033[1;37;40m")
    print("normalised vector: " + str(testvector))
    print("\033[1;32;40mpass\033[1;37;40m")

    print("Attempt to normalise the zero vector")
    print(normalise_vector(np.array([0, 0, 0, 0, 0])))
    print("\033[1;32;40mpass\033[1;37;40m")

    print("Attempt to normalise list")
    print(normalise_vector([3, 4, 0, 0, 0]))
    print("\033[1;32;40mpass\033[1;37;40m")

    print("Attempt to normalise empty vector")
    print(normalise_vector(np.array([])))
    print("\033[1;32;40mpass\033[1;37;40m")

    print("Attempt to normalise None")
    print(normalise_vector(None))
    print("\033[1;32;40mpass\033[1;37;40m")
    print()
    print()


def main():
    """
    Run some basic tests
    """
    test_normalise_vector()

    test_vector_distance()

    test_read_dictionary()

    test_count_occurances()

    test_create_vector_dictionary()

if __name__ == '__main__':
    main()
