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
            min_val = float("inf")
            index = -1
            i = 0
            for vec in self.vectors:
                # dist = vector_distance(x, vec)
                # Dot product is much faster
                dist = -1 * np.dot(x, vec)
                if dist < min_val:
                    min_val = dist
                    index = i
                i = i + 1

            # find corresponding filename
            file = self.files[index]
            ret_list.append(file)
        return np.array(ret_list)
