import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--f_dim', type=int, default=9000, help='Dimension of the input feature vector to the classifier')
parser.add_argument('--vocab_size', type=int, default=9000, help='Size of the vocabulary')
parser.add_argument('--num_iter', type=int, default=50, help='Number of iterations to be run for training')
parser.add_argument('--lr', type=float, default=0.1, help='Learning rate of perceptron')
parser.add_argument('--bin_feats', type=bool, default=True, help='Use binary word features')
#Add any arguments you would like to pass to the classifier (all hyperparameters go here!)

parser.add_argument('--epoch', type=int, default=50, help='Iterations to train perceptron')
parser.add_argument('--epoch_avg', type=int, default=40, help='Iterations to train perceptron')
parser.add_argument('--learning_rate', type=float, default=0.5, help='rate of learning for weights')
parser.add_argument('--stop_early', type=bool, default=False, help='Stop before epoch limit, if perceptron is complete')

args = parser.parse_args()
