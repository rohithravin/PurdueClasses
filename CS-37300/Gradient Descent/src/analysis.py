from classifier import BinaryClassifier
from bgd import BGDHinge,BGDLog,BGDHingeReg,BGDLogReg
from sgd import SGDHinge,SGDLog,SGDHingeReg,SGDLogReg
from utils import read_data, build_vocab
import utils
from config import args
import matplotlib.pyplot as plt

filepath = '../data/given/'

build_vocab(filepath, vocab_size=args.vocab_size)
train_data, test_data = read_data(filepath)

training_acc = []
testing_acc =[]

for epoch in range(1,500,25):
    args.num_iter = epoch
    print(args.num_iterf)
    bgd_l_classifier = SGDHingeReg(args)
    bgd_l_classifier.fit(train_data)
    acc, prec, rec, f1 = bgd_l_classifier.evaluate(test_data)
    testing_acc.append(acc)
    acc, prec, rec, f1 = bgd_l_classifier.evaluate(train_data)
    training_acc.append(acc)

plt.figure()
plt.plot(range(0,500,25), training_acc, label ='Training Accuracy',color='g')
plt.plot(range(0,500,25), testing_acc, label ='Testing Accuracy',color='orange')
plt.xlabel('Epoch')
plt.ylabel('Accuracy')
plt.title('Rohith Ravindranath')
plt.suptitle('SGD Hinge w/ Regularization (Training and Testing Accuracy)')
plt.legend()
plt.show()
