import os
import unittest
import numpy as np

from model.predictor import *
from common.constants import KEYWORDS_FILE

from common.keyword_vector import normalise_vector, vector_distance

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

        testvec1 = normalise_vector(np.array([1, 1, 1]))
        testvec2 = normalise_vector(np.array([1, 3, 10]))

        print("distance to 1")
        print("vec1:", vector_distance(testvec1, vector1))
        print("vec2:", vector_distance(testvec2, vector1))
        print()
        print("distance to 2")
        print("vec1:", vector_distance(testvec1, vector2))
        print("vec2:", vector_distance(testvec2, vector2))
        print()
        print("distance to 3")
        print("vec1:", vector_distance(testvec1, vector3))
        print("vec2:", vector_distance(testvec2, vector2))
        print()

        predictor.fit(vectors, files)
        prediction = predictor.predict(np.array([]))
        self.assertEqual(len(prediction), 0)
        prediction = predictor.predict(np.array([testvec1]))
        self.assertEqual(prediction[0], "file2")
        self.assertEqual(len(prediction), 1)
        prediction = predictor.predict(np.array([testvec1, testvec2]))
        print("Prediction:")
        print(prediction)
        self.assertEqual(prediction[0], "file2")
        self.assertEqual(prediction[1], "file1")
        self.assertEqual(len(prediction), 2)


if __name__ == '__main__':
    unittest.main()
