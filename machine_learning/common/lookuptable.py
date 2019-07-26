"""
j
A Module for fetching helpfiles, creating vectors for each and bundling
these up in a lookup table.
"""

# System imports
import os

# Third party imports
import tarfile
import numpy as np
from six.moves import urllib

# local imports
from common.keyword_vector import count_occurances, read_dictionary, \
        normalise_vector
from common.constants import HELP_FILE_URL, HELP_FILE_PATH, SINGULAR_BIN, \
                        EXTRACT_SCRIPT, KEYWORDS_FILE, HELPFILE_NPY, \
                        VECTORS_NPY


def fetch_tbz2_data(tbz2_url=HELP_FILE_URL, data_path=HELP_FILE_PATH,
                    file_name="helpfiles.tbz2"):
    """
    Download data from a given url, extract to a path provided.
    """
    if not os.path.isdir(data_path):
        os.makedirs(data_path)
    tbz2_path = os.path.join(data_path, file_name)
    urllib.request.urlretrieve(tbz2_url, tbz2_path)

    tbz2_file = tarfile.open(tbz2_path)
    tbz2_file.extractall(path=data_path)
    tbz2_file.close()


def get_list_of_htm_files(path=os.path.join(HELP_FILE_PATH, "html")):
    """
    Return a list of htm files in the given path
    """
    files = [f for f in os.listdir(path) if f.endswith("htm")]
    files.sort()
    return files


def extract_keywords():
    """
    Run Singular script to extract current keywords and save as file
    'keywords.txt'
    """
    os.system(SINGULAR_BIN + " " + EXTRACT_SCRIPT)


def create_table(dictionary=None):
    """
    Get a list of helpfiles, and generate a word occurance vector for each.
    """
    if dictionary is None:
        dictionary = read_dictionary(KEYWORDS_FILE)
    vectors = []

    if not os.path.isfile(VECTORS_NPY) or not os.path.isfile(HELPFILE_NPY):
        file_list = np.array(get_list_of_htm_files())
        np.save(HELPFILE_NPY, file_list)

        for file in file_list:
            vector = count_occurances(os.path.join(HELP_FILE_PATH, "html",
                                                   file),
                                      dictionary,
                                      normalise=False)
            vectors.append(vector)
        vectors = np.array(vectors)
        np.save(VECTORS_NPY, vectors)
    else:
        vectors = np.load(VECTORS_NPY)
        file_list = np.load(HELPFILE_NPY)
    for vector in vectors:
        normalise_vector(vector)

    return (vectors, file_list)


def main():
    """
    Run some tests to check if the functions work.
    """
    fetch_tbz2_data()
    for file in get_list_of_htm_files():
        print(file)
    extract_keywords()
    vectors, files = create_table()
    dictionary = read_dictionary(KEYWORDS_FILE)
    test_vec = count_occurances(os.path.join(HELP_FILE_PATH, "html",
                                             files[1]), dictionary)
    print(test_vec == vectors[1])


if __name__ == '__main__':
    main()
