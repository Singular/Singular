import os
import unittest
import numpy as np
import cProfile

from common.lookuptable import *
from common.keyword_vector import count_occurances
from common.constants import KEYWORDS_FILE, VECTORS_NPY, HELP_FILE_PATH

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

    def test_init_table_on_system(self):
        tbz2_path = os.path.join(HELP_FILE_PATH, "helpfiles.tbz2")
        os.remove(tbz2_path)
        os.remove(KEYWORDS_FILE)
        os.remove(VECTORS_NPY)

        init_table_on_system()

        self.assertTrue(os.path.isfile(tbz2_path))
        self.assertTrue(os.path.isfile(KEYWORDS_FILE))
        self.assertTrue(os.path.isfile(VECTORS_NPY))

        init_table_on_system()

    def test_is_lookup_initialised(self):
        tbz2_path = os.path.join(HELP_FILE_PATH, "helpfiles.tbz2")
        os.remove(tbz2_path)
        self.assertFalse(is_lookup_initialised())
        init_table_on_system()
        self.assertTrue(is_lookup_initialised())

        os.remove(KEYWORDS_FILE)
        self.assertFalse(is_lookup_initialised())
        init_table_on_system()
        self.assertTrue(is_lookup_initialised())

        os.remove(VECTORS_NPY)
        self.assertFalse(is_lookup_initialised())
        init_table_on_system()
        self.assertTrue(is_lookup_initialised())
        

if __name__ == '__main__':
    #cProfile.run("unittest.main()")
    unittest.main()
