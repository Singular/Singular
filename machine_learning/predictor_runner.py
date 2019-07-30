"""
A script to demonstrate that how predictor works
"""

import os
import sys
import numpy as np
from model.predictor import *

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

    start = time.time()
    test_vec = count_occurances("extract.lib", dictionary)
    prediction = predictor.predict(np.array([test_vec]))
    end = time.time()
    print(end - start, "seconds to make prediction")
    print(prediction)
    print()

    print("prediction for zero vector")
    start = time.time()
    zerovec = np.zeros(len(dictionary))
    prediction = predictor.predict(np.array([zerovec]))
    end = time.time()
    print(end - start, "seconds to make prediction")
    print(prediction)
    print()

    if len(sys.argv) >= 2:
        for i in range(len(sys.argv)):
            if i == 0:
                continue
            if not os.path.isfile(sys.argv[i]):
                continue
            print("predicting for file", sys.argv[i])
            start = time.time()
            test_vec = count_occurances(sys.argv[i], dictionary)
            prediction = predictor.predict(np.array([test_vec]))
            end = time.time()
            print(end - start, "seconds to make prediction")
            print(prediction)
            print()

if __name__ == '__main__':
    main()
