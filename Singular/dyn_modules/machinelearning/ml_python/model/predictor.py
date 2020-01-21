"""
Define the predictor class for classifying according to help page.
"""

# import cProfile

# Third party imports
import numpy as np
from sklearn.base import BaseEstimator, ClassifierMixin


class HelpPagePredictor(BaseEstimator, ClassifierMixin):
    """
    Classifier to assign the given input to a Singular helppage.
    """
    def __init__(self):
        """
        Define attributes
        """
        self.vectors = None
        self.files = None


    def fit(self, X, y): # pylint: disable=invalid-name
        """
        Setup the correspondence of vectors to help-files
        """
        assert X is not None, "Please provide data for X"
        assert y is not None, "Please provide data for y"
        self.vectors = X
        self.files = y
        return self


    def predict(self, X): # pylint: disable=invalid-name
        """
        Classify the input vectors
        """
        assert X is not None, "Please provide data for X"
        ret_list = []
        for x in X: # pylint: disable=invalid-name
            # find the closest vector
            indices = [-1, -1, -1, -1, -1]
            vals = [float("inf"), float("inf"),
                    float("inf"), float("inf"),
                    float("inf")]
            i = 0
            for vec in self.vectors:
                # dist = vector_distance(x, vec)
                # Dot product is much faster
                dist = -1 * np.dot(x, vec)
                # go through current list
                for j in range(5):
                    # first entry with greater dist is the insertion position
                    if dist < vals[j]:
                        k = (5 - 1)
                        # shift the values to the right, discarding the last
                        while k > j:
                            indices[k] = indices[k-1]
                            vals[k] = vals[k-1]
                            k = k - 1
                        # insert new value
                        indices[j] = i
                        vals[j] = dist
                        break
                i = i + 1

            # find corresponding filenames
            files = []
            for i in range(5):
                files.append(self.files[indices[i]])
            ret_list.append(files)
        return np.array(ret_list)
