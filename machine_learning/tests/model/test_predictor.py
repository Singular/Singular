import os
import unittest
import numpy as np

from model.predictor import *
from common.constants import KEYWORDS_FILE

class TestPredictionMethods(unittest.TestCase):

    def test_fit(self):
        predictor = HelpPagePredictor()

        self.assertRaises(AssertionError,
                          predictor.fit,
                          None,
                          np.array([]))

        self.assertRaises(AssertionError,
                          predictor.fit,
                          np.array([]),
                          None)
        predictor.fit(np.array([]),np.array([]))

    def test_predict(self):
        predictor = HelpPagePredictor()
        vector1 = normalise_vector(np.array([1, 4, 10]))
        vector2 = normalise_vector(np.array([2, 3, 1]))
        vector3 = normalise_vector(np.array([3, 9, 3]))

        vectors = np.array([vector1, vector2, vector3])
        files = np.array(["file1", "file2", "file3"])

        testvec = normalise_vector(np.array([1, 1, 1]))

        print("distance to 1")
        print(vector_distance(testvec, vector1))
        print()
        print("distance to 2")
        print(vector_distance(testvec, vector2))
        print()
        print("distance to 3")
        print(vector_distance(testvec, vector3))
        print()

        predictor.fit(vectors, files)
        prediction = predictor.predict(np.array([testvec]))
        print("Prediction:")
        print(prediction)
        self.assertEqual(prediction[0], "file2")


if __name__ == '__main__':
    unittest.main()
