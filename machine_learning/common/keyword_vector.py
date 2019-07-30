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
