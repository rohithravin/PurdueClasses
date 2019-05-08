#!/usr/bin/env python
# coding: utf-8
"""

- Original Version

    Author: Susheel Suresh
    Last Modified: 04/03/2019

"""

from classifier import BinaryClassifier
from sgd import SGDHinge,SGDLog,SGDHingeReg,SGDLogReg
from utils import read_data, build_vocab
import utils
import math

from config import args

if __name__ == '__main__':
    filepath = '../data/given/'

    build_vocab(filepath, vocab_size=args.vocab_size)
    train_data, test_data = read_data(filepath)


    sgd_l_classifier = SGDLog(args)
    sgd_l_classifier.fit(train_data)
    acc, prec, rec, f1 = sgd_l_classifier.evaluate(test_data)
    print('\nSGD Log Loss (No Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))


    sgd_l_r_classifier = SGDLogReg(args)
    sgd_l_r_classifier.fit(train_data)
    acc, prec, rec, f1 = sgd_l_r_classifier.evaluate(test_data)
    print('\nSGD Log Loss (With Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))

    
    sgd_h_classifier = SGDHinge(args)
    sgd_h_classifier.fit(train_data)
    acc, prec, rec, f1 = sgd_h_classifier.evaluate(test_data)
    print('\nSGD Hinge Loss (No Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))


    sgd_h_r_classifier = SGDHingeReg(args)
    sgd_h_r_classifier.fit(train_data)
    acc, prec, rec, f1 = sgd_h_r_classifier.evaluate(test_data)
    print('\nSGD Hinge Loss (With Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))
