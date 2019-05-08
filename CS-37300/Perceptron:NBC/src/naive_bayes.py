#!/usr/bin/env python
# coding: utf-8
"""© 2019 Rajkumar Pujari All Rights Reserved

- Original Version

    Author: Rajkumar Pujari
    Last Modified: 03/12/2019

"""

#np.float128()
from classifier import BinaryClassifier
from collections import Counter
import pandas as pd
import numpy as np
import re
import string
import math
from utils import vocab
import utils

class NaiveBayes(BinaryClassifier):

    def __init__(self, args):
        self.positive_prob = 0
        self.negative_prob = 0
        self.positive_features = None
        self.negative_features = None

    def fit(self, train_data):
        features = utils.get_feature_vectors(train_data[0], binary = False)
        features_df = pd.DataFrame(features,columns  = vocab.keys())
        features_df['Label'] = train_data[1]

        self.positive_prob = len(features_df.loc[features_df['Label'] == 1]) / float(len(features_df))
        self.negative_prob = len(features_df.loc[features_df['Label'] == -1]) / float(len(features_df))

        self.positive_features = features_df.loc[features_df['Label'] == 1]
        self.negative_features = features_df.loc[features_df['Label'] == -1]
        self.positive_features = self.positive_features.drop(columns=['Label'], axis = 1)
        self.negative_features = self.negative_features.drop(columns=['Label'], axis = 1)

    def class_prediction(self, text, feature_df, class_prob, class_label ):
        prob = np.float128(0)
        ex_str = '<BOS> ' + text.strip().lower() + ' <EOS>'
        tokens = ex_str.split()

        for tok in tokens:
            if tok in feature_df.columns:
                n_k = 0
                n_k = feature_df[tok].sum()
                prob += np.float128(math.log( (n_k +1)  ))
        prob += np.float128(math.log( class_prob))
        return prob

    def predict(self, test_x):
        predicted_labels = []

        """
        n_1 = 0
        for col in self.positive_features.columns:
            n_1 = n_1 + self.positive_features[col].sum()

        n__1 = 0
        for col in self.negative_features.columns:
            n__1 = n__1 + self.negative_features[col].sum()
        """

        for doc in test_x:

            pos_prob = self.class_prediction(doc,self.positive_features, self.positive_prob, 1 )
            neg_prob = self.class_prediction(doc,self.negative_features, self.negative_prob, -1 )

            if pos_prob <  neg_prob :
                predicted_labels.append(-1)
            else:
                predicted_labels.append(1)
        return predicted_labels
