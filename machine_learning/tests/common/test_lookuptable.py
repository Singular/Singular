import os
import unittest
import numpy as np
import cProfile

from common.lookuptable import *
from common.keyword_vector import count_occurances
from common.constants import KEYWORDS_FILE

class TestLookuptableMethods(unittest.TestCase):

    def test_get_list_of_htm_files(self):
        os.system("rm -r " + HELP_FILE_PATH)
        fetch_tbz2_data()
        fetch_tbz2_data()
        files = get_list_of_htm_files()
        self.assertGreater(len(files), 0)

    def test_extract_keywords(self):
        extract_keywords()
        self.assertTrue(os.path.isfile(KEYWORDS_FILE))

    def test_create_table(self):
        dictionary = read_dictionary(KEYWORDS_FILE)
        vectors, files = create_table(dictionary, attempt_cached=False)
        vectors1, files1 = create_table()
        self.assertTrue((vectors == vectors1).all())
        self.assertTrue((files == files1).all())

        dictionary = read_dictionary(KEYWORDS_FILE)
        test_vec = count_occurances(os.path.join(HELP_FILE_PATH, "html",
                                                 files[1]), dictionary)
        self.assertTrue((test_vec == vectors[1]).all())

if __name__ == '__main__':
    #cProfile.run("unittest.main()")
    unittest.main()
