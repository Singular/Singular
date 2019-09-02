"""
A script to demonstrate that how predictor works
"""

import os
import sys
import time
import numpy as np

from model.predictor import HelpPagePredictor
from common.keyword_vector import read_dictionary, count_occurances
from common.lookuptable import create_table
from common.constants import KEYWORDS_FILE

def find_prediction(filename):
    """
    Given a file name as string, get the predicted help page name
    """
    dictionary = read_dictionary(KEYWORDS_FILE)

    start = time.time()
    vectors, file_list = create_table(dictionary=dictionary)
    end = time.time()
    print(end - start, "seconds to create_table")

    start = time.time()
    pred = get_prediction(filename, dictionary, vectors, file_list)
    end = time.time()
    print(end - start, "seconds to make prediction.")
    return pred

def get_prediction(filename, dictionary, vectors, file_list):
    """
    Train a predictor, get the predicted help page name
    """
    predictor = HelpPagePredictor()
    predictor.fit(vectors, file_list)

    filename = os.path.expanduser(filename)
    test_vec = count_occurances(filename, dictionary)
    prediction = predictor.predict(np.array([test_vec]))
    return prediction[0]


def main():
    """
    Run some basic tests
    """
    print("Running some tests")

    dictionary = read_dictionary(KEYWORDS_FILE)

    start = time.time()
    vectors, file_list = create_table(dictionary=dictionary)
    end = time.time()
    print(end - start, "seconds to create_table")

    predictor = HelpPagePredictor()
    predictor.fit(vectors, file_list)

    print("prediction for zero vector")
    start = time.time()
    zerovec = np.zeros(len(dictionary))
    prediction = predictor.predict(np.array([zerovec]))
    end = time.time()
    print(end - start, "seconds to make prediction")
    print(prediction)
    print()

    prediction = get_prediction("extract.lib",
                                dictionary,
                                vectors,
                                file_list)
    print(prediction)
    print()


    if len(sys.argv) >= 2:
        for i in range(len(sys.argv)):
            if i == 0:
                continue
            if not os.path.isfile(sys.argv[i]):
                continue

            print("predicting for file", sys.argv[i])
            prediction = get_prediction(sys.argv[i],
                                        dictionary,
                                        vectors,
                                        file_list)
            print(prediction)
            print()

if __name__ == '__main__':
    main()
