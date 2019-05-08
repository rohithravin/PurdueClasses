#!/usr/bin/env python
# coding: utf-8
"""© 2019 Rajkumar Pujari All Rights Reserved

- Original Version

    Author: Rajkumar Pujari
    Last Modified: 03/12/2019

"""

from classifier import BinaryClassifier
import utils
import random

class Perceptron(BinaryClassifier):


    def __init__(self, args):
        self.learning_rate = args.learning_rate
        self.epoch = args.epoch_avg
        self.stop_early = args.stop_early
        self.bias = 0.0
        self.weights = []
        return None


    def predict_label(self, inputs, weights, bias):
        activation = 0.0
        for x in range(len(inputs)):
            activation += inputs[x] * weights[x]
        activation += bias
        if activation >= 0.0:
            return 1.0
        else:
            return -1.0

    def check_accuracy(self, data, weights, labels, bias , indices):
        correct = 0.0
        for x in indices:
            vector = data[x]
            pred_label = self.predict_label(vector, weights, bias)
            if (pred_label == labels[x]):
                correct = correct + 1
        return correct / float(len(data))


    def fit(self, train_data):

        features = utils.get_feature_vectors(train_data[0], binary = True)
        labels = train_data[1]
        bias = 0.0
        weights = [0.0] * len(features[0])
        feature_indices_random = [x for x in range(0,len(features))]
        random.shuffle(feature_indices_random)

        for x in range(self.epoch):
            curr_accuracy = self.check_accuracy(features, weights, labels, bias, feature_indices_random )
            if curr_accuracy == 1.0:
                break
            for y in feature_indices_random:
                feature_vector = features[y]
                pred_label = self.predict_label(feature_vector, weights, bias)
                error =  labels[y] - pred_label
                if error != 0:
                    for z in range(len(weights)):
                        weights[z] = weights[z]  +  ( feature_vector[z] *  error *  self.learning_rate)
                    bias  = bias + ( error *  self.learning_rate )
        self.bias = bias
        self.weights = weights

    def predict(self, test_x):
        features = utils.get_feature_vectors(test_x, binary = True)
        predicted_labels = []
        for x in range(len(features)):
            vector = features[x]
            pred_label = self.predict_label(vector, self.weights, self.bias)
            predicted_labels.append(pred_label)
        return predicted_labels


class AveragedPerceptron(BinaryClassifier):

    def __init__(self, args):
        self.learning_rate = args.learning_rate
        self.epoch = args.epoch_avg
        self.stop_early = args.stop_early
        self.bias = 0.0
        self.weights = []
        return None

    def predict_label(self, inputs, weights, bias):
        activation = 0.0
        for x in range(len(inputs)):
            activation += inputs[x] * weights[x]
        activation += bias
        if activation >= 0.0:
            return 1.0
        else:
            return -1.0

    def check_accuracy(self, data, weights, labels, bias , indices):
        correct = 0.0
        for x in indices:
            vector = data[x]
            pred_label = self.predict_label(vector, weights, bias)
            if (pred_label == labels[x]):
                correct = correct + 1
        return correct / float(len(data))

    def fit(self, train_data):
        features = utils.get_feature_vectors(train_data[0], binary = True)
        labels = train_data[1]
        bias = 0.0
        weights = [0.0] * len(features[0])
        feature_indices_random = [x for x in range(0,len(features))]
        random.shuffle(feature_indices_random)
        survival = 1
        for x in range(self.epoch):
            curr_accuracy = self.check_accuracy(features, weights, labels, bias, feature_indices_random )
            if curr_accuracy == 1.0:
                break
            for y in feature_indices_random:
                feature_vector = features[y]
                pred_label = self.predict_label(feature_vector, weights, bias)
                error =  labels[y] - pred_label
                if error == 0:
                    survival = survival + 1
                else:
                    for z in range(len(weights)):
                        temp_weight = weights[z]  +  ( feature_vector[z] *  error *  self.learning_rate)
                        weights[z] = ((survival * weights[z]) + temp_weight ) / float((survival + 1))
                    bias  = bias + (  error *  self.learning_rate )
                    survival = 1
        self.bias = bias
        self.weights = weights

    def predict(self, test_x):
        features = utils.get_feature_vectors(test_x, binary = True)
        predicted_labels = []
        for x in range(len(features)):
            vector = features[x]
            pred_label = self.predict_label(vector, self.weights, self.bias)
            predicted_labels.append(pred_label)
        return predicted_labels
