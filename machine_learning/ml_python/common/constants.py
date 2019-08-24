"""
A constants file for our Singular python subproject
"""
import os

HOME_DIR       = os.path.expanduser("~/.singular")
SINGULAR_BIN   = os.path.expanduser("~/Singular/Singular4/bin/Singular")
EXTRACT_SCRIPT = "ml_singular/extract.lib"
KEYWORDS_FILE  = os.path.expanduser("~/.singular/keywords.txt")
VECTORS_NPY    = os.path.expanduser("~/.singular/vectors.npy")
HELPFILE_NPY   = os.path.expanduser("~/.singular/helpfilelist.npy")

HELP_FILE_URL  = "ftp://jim.mathematik.uni-kl.de/pub/Math/Singular/src/4-1-2/doc.tbz2"
HELP_FILE_PATH = os.path.join(os.path.expanduser("~/.singular"), 
        "helpfiles", "singular")
