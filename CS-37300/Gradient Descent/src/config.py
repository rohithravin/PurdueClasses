import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--f_dim', type=int, default=10000, help='Dimension of the input feature vector to the classifier')
parser.add_argument('--vocab_size', type=int, default=10000, help='Size of the vocabulary')
parser.add_argument('--num_iter', type=int, default=500, help='Number of iterations to be run for training')
parser.add_argument('--lr_bgd', type=float, default=0.001, help='Learning rate for gradient descent')
parser.add_argument('--bin_feats', type=bool, default=True, help='Use binary word features')
parser.add_argument('--lambdas', type=float, default=0.1, help='Regularization')


parser.add_argument('--lr_sgd', type=float, default=0.001, help='Learning rate for stochastic gradient descent')
parser.add_argument('--num_iter_sgd', type=int, default=500, help='Number of iterations to be run for training')

#Add any arguments you would like to pass to the classifier (all hyperparameters go here!)
args = parser.parse_args()
