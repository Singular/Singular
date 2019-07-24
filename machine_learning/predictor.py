"""
Define the predictor class for classifying according to help page.
"""
# Third party imports
from sklearn.base import BaseEstimator, ClassifierMixin

# Local imports
from common.keyword_vector import vector_distance

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
        self.vectors = X
        self.files = y
        return self

    def predict(self, X): # pylint: disable=invalid-name
        """
        Classify the input vectors
        """
        ret_list = []
        for x in X: # pylint: disable=invalid-name
            # find the closest vector

            min_val = float("inf")
            min_vec = None
            for vec in self.vectors:
                dist = vector_distance(x, vec)
                if dist < min_val:
                    min_val = dist
                    min_vec = vec

            # find corresponding filename
            index = self.vectors.index(min_vec)
            file = self.files[index]
            ret_list.append(file)
        return ret_list
