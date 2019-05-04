#!/usr/bin/python3

"""
A file to extract the keywords for Singular
"""

import os
import sys

def create_keyword_list_file(infile_name, outfile_name):
    """
    Take a given input file, extract the correct "keyword" on each line,
    and write the keywords to a file. 
    """
    if not os.path.isfile(infile_name):
        print("Please provide a valid input file as argument")
        return 1

    with open(infile_name, "r") as infile, open(outfile_name, "w+") as outfile:
        line = infile.readline()
        while not line == "":
            vals = [x.strip() for x in line.split('"')]
            keyword = vals[1]
            print(keyword)
            outfile.write(keyword + "\n")
            line = infile.readline()
    return 0
    
if __name__ == '__main__':
    if len(sys.argv) != 3:
        print("Usage: ")
        print(sys.argv[0] + " <infile_name> <outfile_name>")
        sys.exit(1)

    sys.exit(create_keyword_list_file(sys.argv[1], sys.argv[2]))
