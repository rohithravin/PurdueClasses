#!/usr/bin/env python
# coding: utf-8
"""

- Original Version

    Author: Susheel Suresh
    Last Modified: 04/03/2019

"""

from classifier import BinaryClassifier
from utils import get_feature_vectors
import numpy as np
import random
from numpy import linalg as LA
import math

class BGDHinge(BinaryClassifier):

    def __init__(self, args):
        self.learning_rate = args.lr_bgd
        self.epoch = args.num_iter
        self.binary_features = args.bin_feats
        self.lam = args.lambdas
        self.f_weights = None
        self.f_bias = None

    def fit(self, train_data):

        indices = [x for x in range(len(train_data[0]))]
        random.shuffle(indices)
        train_data = ([train_data[0][i] for i in indices],[train_data[1][i] for i in indices])
        
        training_classifications = np.array(train_data[1])
        training_features = np.array(get_feature_vectors(train_data[0], self.binary_features))
        weights_vec = np.zeros(len(training_features[0]))
        bias = 0.0
        for x in range(self.epoch):
            gradient_weights_vec = np.zeros(len(training_features[0]))
            gradient_bias = 0.0
            for feature_vec,label in zip(training_features,training_classifications ):
                if label * (np.dot(weights_vec,feature_vec ) + bias) <= 1:
                    gradient_weights_vec += label*feature_vec
                    gradient_bias +=  label
            weights_vec += self.learning_rate*gradient_weights_vec
            bias = bias + self.learning_rate*gradient_bias
        self.f_weights = weights_vec
        self.f_bias = bias

    def predict(self, test_x):
        data = np.array(get_feature_vectors(test_x,self.binary_features ))
        return [np.sign(np.dot(self.f_weights,np.array(d)) + self.f_bias) for d in data]

class BGDLog(BinaryClassifier):

    def __init__(self, args):
        self.learning_rate = args.lr_bgd
        self.epoch = args.num_iter
        self.f_dim = args.f_dim
        self.binary_features = args.bin_feats
        self.f_weights = np.array([0.0] * args.f_dim)
        self.f_bias = 0.0

    def fit(self, train_data):

        #shuffle
        indices = [x for x in range(len(train_data[0]))]
        random.shuffle(indices)
        train_data = ([train_data[0][i] for i in indices],[train_data[1][i] for i in indices])

        training_features = np.array(get_feature_vectors(train_data[0], self.binary_features))
        training_classifications = np.array(train_data[1])
        for x in range(self.epoch):

            bias_gradient = 0
            gradient_vec = np.zeros(self.f_dim)

            for y in range(len(training_features)):
                input = training_features[y]
                label = 0
                if training_classifications[y] == -1:
                    label = 0
                else:
                    label = 1

                sigmoid_func = 1 / (1 + np.exp(-1 * (np.dot(self.f_weights,input) + self.f_bias)))
                gradient_vec += np.dot(input, (sigmoid_func - label))
                bias_gradient -= (1/len(training_features[0]) * (sigmoid_func-label))

            gradient_vec = np.dot(gradient_vec, -self.learning_rate / len(training_features[0]))
            norm = LA.norm(gradient_vec)
            if norm == 0.0:
                break
            gradient_vec = np.dot(gradient_vec, 1/norm)
            self.f_weights += gradient_vec
            self.f_bias = bias_gradient

    def predict(self, test_x):
        training_features = np.array(get_feature_vectors(test_x, self.binary_features))
        predict_labels = []
        for x in range(len(training_features)):
            sig = 1 / (1 + math.exp(-1 * (np.dot(np.transpose(self.f_weights),training_features[x]) + self.f_bias) ))
            if sig > 0.5:
                predict_labels.append(1)
            else:
                predict_labels.append(-1)
        return predict_labels

class BGDHingeReg(BinaryClassifier):

    def __init__(self, args):
        self.learning_rate = args.lr_bgd
        self.epoch = args.num_iter
        self.binary_features = args.bin_feats
        self.lam = 0.5
        self.f_weights = None
        self.f_bias = None

    def fit(self, train_data):
        indices = [x for x in range(len(train_data[0]))]
        random.shuffle(indices)
        train_data = ([train_data[0][i] for i in indices],[train_data[1][i] for i in indices])

        training_classifications = np.array(train_data[1])
        training_features = np.array(get_feature_vectors(train_data[0], self.binary_features))
        weights_vec = np.zeros(len(training_features[0]))
        bias = 0.0
        for x in range(self.epoch):
            gradient_weights_vec = np.zeros(len(training_features[0]))
            gradient_bias = 0.0
            for feature_vec,label in zip(training_features,training_classifications ):
                if label * (np.dot(weights_vec,feature_vec ) + bias) <= 1:
                    gradient_weights_vec += label*feature_vec
                    gradient_bias +=  label
            gradient_weights_vec -= weights_vec*self.lam
            weights_vec += self.learning_rate*gradient_weights_vec
            bias = bias + self.learning_rate*gradient_bias
        self.f_weights = weights_vec
        self.f_bias = bias

    def predict(self, test_x):
        data = np.array(get_feature_vectors(test_x,self.binary_features ))
        return [np.sign(np.dot(self.f_weights,np.array(d)) + self.f_bias) for d in data]

class BGDLogReg(BinaryClassifier):

    def __init__(self, args):
        self.learning_rate = args.lr_bgd
        self.epoch = args.num_iter
        self.f_dim = args.f_dim
        self.binary_features = args.bin_feats
        self.f_weights = np.array([0.0] * args.f_dim)
        self.f_bias = 0.0
        self.lam = args.lambdas

    def fit(self, train_data):

        #shuffle
        indices = [x for x in range(len(train_data[0]))]
        random.shuffle(indices)
        train_data = ([train_data[0][i] for i in indices],[train_data[1][i] for i in indices])

        training_features = np.array(get_feature_vectors(train_data[0], self.binary_features))
        training_classifications = np.array(train_data[1])
        for x in range(self.epoch):

            bias_gradient = 0
            gradient_vec = np.zeros(self.f_dim)

            for y in range(len(training_features)):
                input = training_features[y]
                label = 0
                if training_classifications[y] == -1:
                    label = 0
                else:
                    label = 1

                sigmoid_func = 1 / (1 + np.exp(-1 * (np.dot(self.f_weights,input) + self.f_bias)))
                gradient_vec += np.dot(input, (sigmoid_func - label))
                bias_gradient -= (1/len(training_features[0]) * (sigmoid_func-label))

            gradient_vec = np.dot(gradient_vec, -self.learning_rate / len(training_features[0]))
            gradient_vec = np.add(gradient_vec,(self.f_weights * self.lam * (-self.learning_rate )))
            norm = LA.norm(gradient_vec)
            if norm == 0.0:
                break
            #gradient_vec = np.dot(gradient_vec, 1/norm)
            self.f_weights += gradient_vec
            self.f_bias = bias_gradient

    def predict(self, test_x):
        training_features = np.array(get_feature_vectors(test_x, self.binary_features))
        predict_labels = []
        for x in range(len(training_features)):
            sig = 1 / (1 + math.exp(-1 * (np.dot(np.transpose(self.f_weights),training_features[x]) + self.f_bias) ))
            if sig > 0.5:
                predict_labels.append(1)
            else:
                predict_labels.append(-1)
        return predict_labels
