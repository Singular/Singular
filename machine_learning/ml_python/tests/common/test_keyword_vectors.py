import sys
import unittest
import numpy as np

from common.keyword_vector import *

class TestKeywordMethods(unittest.TestCase):

    def test_read_dictionary(self):
        """
        Create test for read_dictionary function
        """
        print("Testing read_dictionary function:")

        print("Non-existant file")
        if sys.version_info[0] == 3:
            self.assertRaises(FileNotFoundError,
                              read_dictionary,
                              "asdfasdf")
        else:
            self.assertRaises(IOError,
                              read_dictionary,
                              "asdfasdf")


        print("Reading default file")
        correct = True
        read_dictionary()
        print()

    def test_count_occurances(self):
        """
        Create test for count_occurances function
        """
        print("Testing count_occurances function:")

        dic = read_dictionary()
        print("Test non-existant file")
        if sys.version_info[0] == 3:
            self.assertRaises(FileNotFoundError,
                              count_occurances,
                              "asdfasdf",
                              dic)
        else:
            self.assertRaises(IOError,
                              count_occurances,
                              "asdfasdf",
                              dic)

        print("Count occurances with None dictionary:")
        self.assertRaises(AssertionError, count_occurances,
                          "../Singular/table.h",
                          None)

        print("Count occurances with empty dictionary:")
        self.assertRaises(AssertionError, count_occurances,
                          "../Singular/table.h",
                          np.array([]))

        print("vector of ../Singular/table.h")
        vec = count_occurances("../Singular/table.h", dic)
        print(vec)
        print("Don't normalise")
        vec = count_occurances("../Singular/table.h", dic, False)
        self.assertGreater(np.linalg.norm(vec), 1)
        print()
        

    def test_create_vector_dictionary(self):
        """
        Create test for create_vector_dictionary function
        """
        print("Testing create_vector_dictionary function:")

        dictionary = read_dictionary()

        print("Create Vector Dictionary with None as dictionary:")
        self.assertRaises(AssertionError, create_vector_dictionary, None)

        print("Create Vector Dictionary with empty dictionary:")
        self.assertRaises(AssertionError,
                          create_vector_dictionary,
                          np.array([]))
        vec_dic = create_vector_dictionary(dictionary)

    def test_vector_distance(self):
        """
        Create test for vector_distance function
        """
        print("Testing vector_distance function:")

        vector1 = np.array([3, 4])
        vector1 = normalise_vector(vector1)
        vector2 = np.array([4, 3])
        vector2 = normalise_vector(vector2)

        print("Distance of vectors of different dimensions:")
        self.assertRaises(AssertionError,
                          vector_distance,
                          np.array([1, 2, 3]),
                          vector1)

        print("Distance same vector: " + str(vector_distance(vector1, vector1)))
        self.assertEqual(vector_distance(vector1, vector1), 0)

        print("Distance different vector: " + str(vector_distance(vector1, vector2)))
        self.assertGreater(vector_distance(vector1, vector2), 0)
        print()

    def test_normalise_vector(self):
        """
        Create test for normalise_vector function
        """
        print("Testing normalise_vector function:")
        testvector = np.array([3, 4])
        testvector = normalise_vector(testvector)
        self.assertEqual(np.linalg.norm(testvector), 1)

        print("Attempt to normalise the zero vector")
        vec = normalise_vector(np.array([0, 0, 0, 0, 0]))
        self.assertEqual(np.linalg.norm(vec), 0)

        print("Attempt to normalise list")
        vec = normalise_vector([3, 4, 0, 0, 0])
        self.assertEqual(np.linalg.norm(vec), 1)

        print("Attempt to normalise empty vector")
        vec = normalise_vector(np.array([]))
        self.assertEqual(np.linalg.norm(vec), 0)

        print("Attempt to normalise None")
        vec = normalise_vector(None)
        self.assertEqual(np.linalg.norm(vec), 0)
        print()


    def test_copy_dictionary(self):
        dic = np.array(["hello", "bye"])
        dic_copy = copy_dictionary(dic)
        self.assertTrue((dic == dic_copy).all())


    def test_copy_vector(self):
        vec = np.array([1, 2, 3])
        vec_copy = copy_vector(vec)
        self.assertTrue((vec == vec_copy).all())


if __name__ == '__main__':
    unittest.main()
