#!/usr/bin/env python
# coding: utf-8
"""

- Original Version

    Author: Susheel Suresh
    Last Modified: 04/03/2019

"""

from classifier import BinaryClassifier
from bgd import BGDHinge,BGDLog,BGDHingeReg,BGDLogReg
from utils import read_data, build_vocab
import utils
from config import args

if __name__ == '__main__':
    filepath = '../data/given/'

    build_vocab(filepath, vocab_size=args.vocab_size)
    train_data, test_data = read_data(filepath)

    
    bgd_l_classifier = BGDLog(args)
    bgd_l_classifier.fit(train_data)
    acc, prec, rec, f1 = bgd_l_classifier.evaluate(test_data)
    print('\nBGD Log Loss (No Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))

    bgd_l_r_classifier = BGDLogReg(args)
    bgd_l_r_classifier.fit(train_data)
    acc, prec, rec, f1 = bgd_l_r_classifier.evaluate(test_data)
    print('\nBGD Log Loss (With Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))


    bgd_h_classifier = BGDHinge(args)
    bgd_h_classifier.fit(train_data)
    acc, prec, rec, f1 = bgd_h_classifier.evaluate(test_data)
    print('\nBGD Hinge Loss (No Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))


    bgd_h_r_classifier = BGDHingeReg(args)
    bgd_h_r_classifier.fit(train_data)
    acc, prec, rec, f1 = bgd_h_r_classifier.evaluate(test_data)
    print('\nBGD Hinge Loss (With Regularization) :')
    print('Accuracy: %.2f, Precision: %.2f, Recall: %.2f, F1: %.2f'%(acc, prec, rec, f1))
