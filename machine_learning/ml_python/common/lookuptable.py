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
from common.keyword_vector import get_vectors, read_dictionary
from common.constants import HELP_FILE_URL, HELP_FILE_PATH, SINGULAR_BIN, \
                        EXTRACT_SCRIPT, KEYWORDS_FILE, HELPFILE_NPY, \
                        VECTORS_NPY, HOME_DIR


def fetch_tbz2_data(tbz2_url=HELP_FILE_URL, data_path=HELP_FILE_PATH,
                    file_name="helpfiles.tbz2"):
    """
    Download data from a given url, extract to a path provided.
    """
    if not os.path.isdir(data_path):
        os.makedirs(data_path)
    tbz2_path = os.path.join(data_path, file_name)
    urllib.request.urlcleanup()
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
    # ensure the homedir exists
    if not os.path.isdir(HOME_DIR):
        os.makedirs(HOME_DIR)

    # extract keywords using the singular script
    os.system(SINGULAR_BIN + " -q " + EXTRACT_SCRIPT +
            " | sort | uniq > " + KEYWORDS_FILE)

    # read from the file created by singular
    dictionary = read_dictionary()

    return dictionary


def create_table(dictionary=None, attempt_cached=True):
    """
    Get a list of helpfiles, and generate a word occurance vector for each.
    """

    if dictionary is None:
        dictionary = read_dictionary(KEYWORDS_FILE)
    vectors = []

    if not os.path.isfile(VECTORS_NPY) or \
            not os.path.isfile(HELPFILE_NPY) or \
            not attempt_cached:
        if not os.path.exists(HOME_DIR):
            os.makedirs(HOME_DIR)
        file_list = np.array(get_list_of_htm_files())
        np.save(HELPFILE_NPY, file_list)

        filenames = []
        for file in file_list:
            filename = os.path.join(HELP_FILE_PATH, "html", file)
            filenames.append(filename)
        filenames = np.array(filenames)
        vectors = get_vectors(filenames, dictionary, normalise=False)
        np.save(VECTORS_NPY, vectors)
    else:
        vectors = np.load(VECTORS_NPY)
        file_list = np.load(HELPFILE_NPY)

    # normalise the vectors
    vectors = vectors / np.sqrt((vectors ** 2).sum(-1))[..., np.newaxis]

    return (vectors, file_list)

def init_table_on_system():
    """
    check whether the various files exist, and create if necessary.
    """
    if not os.path.isdir(HOME_DIR):
        os.makedirs(HOME_DIR)

    # check for and download help files if necessary
    tbz2_path = os.path.join(HELP_FILE_PATH, "helpfiles.tbz2")
    if not os.path.isdir(HELP_FILE_PATH) or not os.path.isfile(tbz2_path):
        fetch_tbz2_data()

    # Use Singular to extract the keywords and save in a file.
    if not os.path.isfile(KEYWORDS_FILE):
        dictionary = extract_keywords()
    else:
        dictionary = None

    if not os.path.isfile(VECTORS_NPY) or not os.path.isfile(HELPFILE_NPY):
        create_table(dictionary=dictionary,
                     attempt_cached=False)

def is_lookup_initialised():
    """
    Check whether the various files exist, return True if so, False
    otherwise.
    """
    retvalue = True
    tbz2_path = os.path.join(HELP_FILE_PATH, "helpfiles.tbz2")
    if not os.path.isdir(HELP_FILE_PATH) or not os.path.isfile(tbz2_path):
        retvalue = False
    if not os.path.isfile(KEYWORDS_FILE):
        retvalue = False
    if not os.path.isdir(HOME_DIR) or \
            not os.path.isfile(VECTORS_NPY) or \
            not os.path.isfile(HELPFILE_NPY):
        retvalue = False

    return retvalue
