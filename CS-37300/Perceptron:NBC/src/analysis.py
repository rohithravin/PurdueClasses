# QUESTION 2


from classifier import BinaryClassifier
from perceptron import Perceptron, AveragedPerceptron
from naive_bayes import NaiveBayes
from utils import read_data, build_vocab
import utils
from config import args
import matplotlib.pyplot as plt

"""
vocab_s = [100, 500, 1000, 5000, 10000, 20000]
acc_avg_per = []
acc_n_b = []
for s in vocab_s:

    args.vocab_size = s
    print(args.vocab_size )
    filepath = '../data/given/'
    build_vocab(filepath, vocab_size=args.vocab_size)
    train_data, test_data = read_data(filepath)

    avg_perc_classifier = AveragedPerceptron(args)
    avg_perc_classifier.fit(train_data)
    acc, prec, rec, f1 = avg_perc_classifier.evaluate(test_data)
    acc_avg_per.append(acc)

    nb_classifier = NaiveBayes(args)
    nb_classifier.fit(train_data)
    acc, prec, rec, f1 = nb_classifier.evaluate(test_data)
    acc_n_b.append(acc)


plt.figure()
plt.plot(vocab_s,acc_avg_per, '-p', color='gray',
             markersize=15, linewidth=4,
             markerfacecolor='white',
             markeredgecolor='gray',)
plt.xlabel('Vocab Size')
plt.ylabel('Accuracy')
plt.title('Rohith Ravindranath (Question 3)' , fontsize = 10)
plt.suptitle('Vocab Size v Accuracy (Avg Perceptron)',  fontsize = 12)
plt.savefig((str('Avg Perceptron (vocab size v acc)') + '.png'))

plt.figure()
plt.plot(vocab_s,acc_n_b, '-p', color='gray',
             markersize=15, linewidth=4,
             markerfacecolor='white',
             markeredgecolor='gray',)
plt.xlabel('Vocab Size')
plt.ylabel('Accuracy')
plt.title('Rohith Ravindranath  (Question 3)' , fontsize = 10)
plt.suptitle('Vocab Size v Accuracy (Naive Bias)',  fontsize = 12)
plt.savefig((str('Naive Bias (vocab size v acc)') + '.png'))


"""

args.vocab_size = 10000
print(args.vocab_size )
filepath = '../data/given/'
build_vocab(filepath, vocab_size=args.vocab_size)
train_data, test_data = read_data(filepath)



num_iter = [1,2,5,10,20,50]
acc_list = []
for iter in num_iter:
    args.epoch_avg = iter
    avg_perc_classifier = AveragedPerceptron(args)
    avg_perc_classifier.fit(train_data)
    acc, prec, rec, f1 = avg_perc_classifier.evaluate(test_data)
    acc_list.append(acc)
plt.figure()
plt.plot(num_iter,acc_list, '-p', color='gray',
             markersize=15, linewidth=4,
             markerfacecolor='white',
             markeredgecolor='gray',)
plt.xlabel('Number of Iterations')
plt.ylabel('Accuracy')
plt.title('Rohith Ravindranath  (Question 2)' , fontsize = 10)
plt.suptitle('# of Iterations v Accuracy (Avg Perceptron)',  fontsize = 12)
plt.savefig((str('Avg Perceptron (iter v acc)') + '.png'))

"""
perc_classifier = Perceptron(args)
perc_classifier.fit(train_data)
"""
