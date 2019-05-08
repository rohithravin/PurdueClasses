##############
# Name: Rohith Ravindranath
# Email: rravind@purdue.edu
#PUID: 0028822977

import numpy as np
import pandas as pd
from sys import argv
import math
import statistics
import operator

"""
    * Tree class holds all the necessary methods that is needed to build, load
    * data, predict, and test accuracy on the decision tree
"""
class Tree():

    """
        * Global variable list that stores all the avaiable features the decision
        * tree and split on, including the last element in the list being the class label
    """
    columns = ['Pclass','Sex','Age','Fare','Embarked','Relatives','IsAlone','Survived']

    """
        * Constructor that holds the type of decision tree is being built
        * Types of decision tress are: vanilla, min_split, depth, and prune
        @param train_data_file
        @param train_label_file
        @param test_data_file
        @param test_label_file
        @param model
    """
    def __init__(self, train_data_file,train_label_file, test_data_file,test_label_file, model):
        self.train_data_file = train_data_file
        self.train_label_file = train_label_file
        self.test_label_file = test_label_file
        self.test_data_file = test_data_file
        self.model = model

    """
        * Given a data file and a data label file, will load the data into a singular pandas dataframe
        * For continous attributes (Age, Fare, and Relatives), the method will
        * convert and classify them into discrete attributes
        @param data_file
        @param data_label
        @return df (pandas dataframe)
    """
    def load_data(self, data_file, data_label):
        attributes_df = pd.read_csv(data_file)
        label_df = pd.read_csv(data_label, header=None)
        #combining the two separate dataframes into one
        df = pd.concat([attributes_df, label_df], axis=1)
        df.columns = ['Pclass','Sex','Age','Fare','Embarked','Relatives','IsAlone','Survived']

        #convert Relatives to discrete variables
        df['Relatives'] = df['Relatives'].mask(((df['Relatives'] == 0) | (df['Relatives'] == 1)), 0)
        df['Relatives'] = df['Relatives'].mask(((df['Relatives'] == 3) | (df['Relatives'] == 2)), 1)
        df['Relatives'] = df['Relatives'].mask(((df['Relatives'] == 6) | (df['Relatives'] == 5) | (df['Relatives'] == 4)), 2)

        #convert Age to discrete variables by finding the 25th, 50th and 75th
        #quartile range and spllitting the classifications accordingly
        age_quantiles = np.percentile(np.array(df['Age'].tolist()), [25,50,75])
        age_quantiles[0] = round(age_quantiles[0])
        age_quantiles[1] = round(age_quantiles[1])
        age_quantiles[2] = round(age_quantiles[2])
        df['Age'] = df['Age'].mask(((df['Age'] >= 0) & (df['Age'] <(age_quantiles[0]) ) ), 0)
        df['Age'] = df['Age'].mask(((df['Age'] >= (age_quantiles[0])) & (df['Age'] <(age_quantiles[1]))), 1)
        df['Age'] = df['Age'].mask(((df['Age'] >= age_quantiles[1]) & (df['Age'] < age_quantiles[2])), 2)
        df['Age'] = df['Age'].mask(((df['Age'] >= (age_quantiles[2]))), 3)

        #convert Fare to discrete variables by finding the 25th, 50th and 75th
        #quartile range and spllitting the classifications accordingly
        fare_quantiles = np.percentile(np.array(df['Fare'].tolist()), [25,50,75])
        fare_quantiles[0] = round(fare_quantiles[0])
        fare_quantiles[1] = round(fare_quantiles[1])
        fare_quantiles[2] = round(fare_quantiles[2])
        df['Fare'] = df['Fare'].mask(((df['Fare'] >= 0) & (df['Fare'] < fare_quantiles[0])), 0)
        df['Fare'] = df['Fare'].mask(((df['Fare'] >= fare_quantiles[0]) & (df['Fare'] < fare_quantiles[1])), 1)
        df['Fare'] = df['Fare'].mask(((df['Fare'] >= fare_quantiles[1]) & (df['Fare'] < fare_quantiles[2])), 2)
        df['Fare'] = df['Fare'].mask(((df['Fare'] >= fare_quantiles[2])), 3)

        return df

    """
        * Given a dataframe (usually a subset of the actual full data), method will calculate the entropy of the data
        * Will calculate entropy based on the Survived column on the dataset
        * Formula is -(survived/length of dataset) *  -log((survived/length of dataset))
        @param data
        @return entropy
    """
    def calc_entropy(self, data):
        #number of rows where survived is 1
        survived = len(data.loc[data['Survived'] == 1])
        #number of rows wehre survived is not 1
        not_survived = len(data) - survived

        #calculate the positive outcomes entropy using the formula
        if(survived != 0):
            ep_1 = float(survived) / float(len(data)) * math.log(float(survived) / float(len(data)), 2)
        else:
            ep_1 = 0

        #calculate the negative outcomes entropy using the formula
        if(not_survived != 0):
            ep_2 = float(not_survived) / float(len(data)) * math.log(float(not_survived) / float(len(data)), 2)
        else:
            ep_2 = 0

        #calculate the final entropy score based on previous two values
        entropy = ((-1) * ep_1) - ep_2

        return entropy

    """
        * Method will split the data set into two separate groups based on the columns and columns value
        * Left group will be rows that equal the column value, and everything else in the right group
        @param data
        @param column
        @param value
        @return left, right (tuple)
    """
    def get_groups(self,data,column,value):
        left = data.loc[data[column] == value]
        right = data.loc[data[column] != value]

        #returning the group as a tuple
        return left,right

    """
        * Given a dataframe and column name, will find the information for that column,
        * along with the best column value to split on based on information_gain of that specific column value
        * Will also return the two split groups (left, right) corresponding to the best splitting column value
        @param data
        @param column
        @return final_ig, final_value, final_groups (tuple)
    """
    def calc_information_gain(self, data, column):
        #calculate the entropy of the whole data set to calculate information gain
        #and compare which column value is best to split on
        full_df_entropy = self.calc_entropy(data)
        final_ig = 0.0
        final_value = 0.0
        final_groups = None

        #get the unique columns values to split on from
        unique_vals = list(set(data[column].tolist()))

        for val in unique_vals:
            #getting the left and right groups for the specific column value
            groups = self.get_groups(data,column,val)
            subtract = 0.0
            for sub_group in groups:
                #calculate entropy for each subgroup
                entropy = self.calc_entropy(sub_group)
                subtract +=  (float(len(sub_group)) / float(len(data))) *  entropy
            #calculate the information_gain of this specifc column value
            ig = full_df_entropy - subtract
            #determine if this column value is the better value to split on, by comparing information_gain values
            if ig > final_ig:
                final_ig = ig
                final_value = val
                final_groups = groups

        return final_ig, final_value, final_groups

    """
        * This method will create a node that splits the given data on the best column and column value
        * Method will use the list of columns and find the information_gain for each of the column and see which
        * column gives the higher information_gain and split on that column
        @param data
        @param columns
        @return node
    """
    def create_split(self, data, columns):
        n_column = ''
        n_value = 0.0
        n_groups = None
        n_ig = 0.0
        for column in columns:
            #find the information_gain for each of the column
            ig, value, groups = self.calc_information_gain(data,column)
            #determine if the information_gain is higher from the save maxed information_gain
            if ig > n_ig:
                n_ig = ig
                n_column = column
                n_value = value
                n_groups = groups
        #if none of the columns give any information_gain then just set all the data to the left group
        if n_ig == 0.0:
            n_groups = (data, None)
        #return the node as a splitting node, if the ig is 0.0, in the split method wil convert node to terminal
        node = Node('s', n_column, n_value, n_ig, n_groups)
        return node

    """
        * Gives a list of numbers, the method will will the most occuring unique number and return it
        * Method is used to calculate the if a terminal node's value should be 1 or 0,
        * since the list will be a list of 1's and 0's from the Survived column
        @param lis
        @return num (1 or 0)
    """
    def find_terminal_val(self,lis):
        #is the list is null or no lis then just return 0
        if (len(lis) == 0 ) or (lis is None):
            return 0
        dict = {}
        #get unique values in list
        unique = set(lis)
        for val in unique:
            #add the number of occurances of the val to the dictionary
            dict[val] = lis.count(val)
        #sort the dictionary by the value
        sorted_x = sorted(dict.items(), key=operator.itemgetter(1))
        #since it is sorted in incraseing order, return the last element in the list of tuples
        return sorted_x[-1][0]

    """
        * Main abstract method that will call the required method in this class to construct the decision tree
        @param training_data
        @param depth
        @param min_split (optional)
        @return root (root node of the decision tree)
    """
    def construct_tree(self,training_data, depth, min_split = -1, v_data = None):
        #create inital split on data to get the root node
        root = self.create_split(training_data,training_data.columns[:-1])
        #call the split tree method that will create the decision tree
        self.split_tree(root, Tree.columns[:-1], depth,1, min_split)
        #if the model is prune, then start with post prunning with reduced error prunning methodology
        if self.model == 'prune' and v_data is not None:
            root = self.prune_tree(root, v_data)
        return root

    """
        * This recursive method will split a node based on column and column value
        * and continue to split until it reaches a terminal node and set that node to a terminal value
        * Method does not return anything since, the nodes wil store the left and
        * right child node therefore, only needing the root node is sufficient
        @param tree
        @param columns
        @param max_depth
        @param depth
        @param min_split
    """
    def split_tree(self, tree, columns, max_depth, depth, min_split):
        #get the left and right groups of the node, in order to determine splitting values for child nodes

        left_group = tree.n_groups[0]
        right_group = tree.n_groups[1]
        sub_columns = columns[:]
    #    if tree.gain != 0:
    #        sub_columns.remove(tree.n_column)
        num_nodes_left = 0
        num_nodes_right = 0


        #if there is no information gain on this node, when all the data is on one
        #side, and then just convert node to terminal nodes
        if  tree.gain == 0.0 or (left_group is  None or right_group is None):
            tree.type = 't'
            #find terminal value based on if the data is on the right or left side
            if left_group is None and right_group is not None:
                term_val  = self.find_terminal_val( right_group['Survived'].tolist())
            if right_group is None and left_group is not None:
                term_val  = self.find_terminal_val( left_group['Survived'].tolist())
            tree.n_value = term_val
            return

        #if the depth reaches the the max depth then just stop here and convert the node into a terminal node
        if depth >= max_depth:
            tree.type = 't'
            #find the terminal value by using the both left and right group and return
            term_val = self.find_terminal_val(left_group['Survived'].tolist() + right_group['Survived'].tolist() )
            tree.n_value = term_val
            return


        #ACTUAL SPITTING STUFF HAPPENS NOW

        #first check if the data on the left is greater that minimum splitting sample
        if (len(left_group) < min_split):
            #if not then convert node to terminal and find the terminal value of the left group
            left_node = Node(type = 't', n_groups = left_group)
            term_val  = self.find_terminal_val( left_group['Survived'].tolist())
            tree.n_left = left_node
            tree.n_left.n_groups = (left_group, None)
            tree.n_left.n_value = term_val
        else:
            #find the best column and column value to split on given the left group
            #and columns name and set the node the the left node of the current node
            tree.n_left = self.create_split(left_group, sub_columns)
            #if there is no gain on splitting in this group then just convert this node to a terminal node
            if tree.gain == 0.0:
                tree.n_left.type = 't'
                tree.n_left.n_value = self.find_terminal_val( left_group['Survived'].tolist())
                tree.n_left.n_groups = (left_group, None)
                tree.n_left.n_column = ''
            else:
                #recursive call this method again but with the left node and incrase in depth
                self.split_tree(tree.n_left, sub_columns,max_depth,depth+1,min_split )

        #first check if the data on the right is greater that minimum splitting sample
        if (len(right_group) < min_split):
            #if not then convert node to terminal and find the terminal value of the right group
            right_node = Node(type = 't', n_groups = right_group)
            term_val  = self.find_terminal_val( right_group['Survived'].tolist())
            tree.n_right = right_node
            tree.n_right.n_groups = (right_group, None)
            tree.n_right.n_value = term_val
        else:
            #find the best column and column value to split on given the right group
            #and columns name and set the node the the right node of the current node
            tree.n_right = self.create_split(right_group, sub_columns)
            if tree.gain == 0.0:
                #if there is no gain on splitting in this group then just convert this node to a terminal node
                tree.n_right.type = 't'
                tree.n_right.n_value = self.find_terminal_val( right_group['Survived'].tolist())
                tree.n_right.n_groups = (right_group, None)
                tree.n_right.n_column = ''
            else:
                #recursive call this method again but with the right node and incrase in depth
                self.split_tree(tree.n_right, sub_columns,max_depth,depth+1,min_split)

    """
        * Given a dataset and the root node a decision tree, run through the tree
        * for each row in the dataframe and save the predictated class label and return it
        @param root
        @param predict_df
        @return lis (labeled class label, predicted class label)
    """
    def predict(self, predict_df, root):
        lis = []
        for index, row in predict_df.iterrows():
            #set ndoe to root, so the pointer of root doesn't change at all
            node = root
            #if node is a splitting node then check if the row should go to the right or left child node
            while node.type == 's':
                if predict_df.loc[index][node.n_column] == node.n_value:
                    node = node.n_left
                else:
                    node = node.n_right
            #if node is a terminal node, get the terminal value and then append to list as a tuple
            lis.append((predict_df.loc[index]['Survived'] , node.n_value))
        return lis

    """
        * Given a list of tuples (actual label, predicted label),
        * count the number of true positives in the list and output the percentage
        @param data
        @return percentage
    """
    def analyze(self, data):
        count = 0
        for x,y in data:
            if x == y:
                count = count + 1
        return count / len(data)

    """
        * Given a value and data, find the error sum of when the terminal value
        * does equal the value in the data frame
        @param leaf_value
        @param data
        @return err
    """
    def get_err(self, leaf_value, data):
        err = 0.0
        for index, row in data.iterrows():
            #compare terminal value with value of class label in specific row
            if leaf_value != data.loc[index]['Survived']:
                err = err + 1
        return float(err)

    """
     * Given a node and a different data set, will determine if the child nodes
     * make a significally statistically difference in information_gain to keep
     * the split or convert the node to terminal node and return the node
     * Method is recursivley called
     @param tree
     @param data
     @return node
    """
    def prune_tree(self, tree, data):
        left_data = None
        right_data = None

        #if either of child nodes are splitting, get the left and right groups
        if tree.n_left.type == 's' or tree.n_right.type == 's':
            #using get groups to get the groups
            split_data = self.get_groups(data, tree.n_column, tree.n_value)
            left_data = split_data[0]
            right_data = split_data[1]

        #if left node is a splitting node, then call the prunning method recursivly  with the left data
        if tree.n_left.type == 's':
            tree.n_left = self.prune_tree(tree.n_left, left_data)

        #if right node is a splitting node, then call the prunning method recursivly  with the right data
        if tree.n_right.type == 's':
            tree.n_right = self.prune_tree(tree.n_right, right_data)

        #if both the child nodes are terminal nodes then try to prune the node
        if tree.n_right.type == 't' and tree.n_left.type == 't':
            #get the groups
            split_data = self.get_groups(data, tree.n_column, tree.n_value)
            left_data = split_data[0]
            right_data = split_data[1]
            left_sum_err = 0.0
            right_sum_err = 0.0

            #if there is left data then get the error value for the left group and left node value
            if len(left_data) != 0:
                left_sum_err = self.get_err(tree.n_left.n_value, left_data)

            #if there is right data then get the error value for the right group and right node value
            if len(right_data) != 0:
                right_sum_err = self.get_err(tree.n_right.n_value, right_data)

            #find the merrge value to not merge these two child nodes together
            err_merge_no = math.pow(left_sum_err,2) + math.pow(right_sum_err,2)

            #find the merrge value to  merge these two child nodes together
            err_merge = math.pow(self.find_terminal_val(data['Survived'].tolist()), 2)

            #if the merge value error is less than the no merge value then convert
            #the node to a terminal node and return it
            if err_merge < err_merge_no:
                node = Node(type = 't', n_groups = data)
                node.n_value = self.find_terminal_val(data['Survived'].tolist())
                return node
            else:
                return tree
        #if both are not terminal nodes then just return the tree
        else:
            return tree

    """
        * This recursive method will count the number of nodes of a give tree
        @param tree
        @return number of nodes and child nodes  at a particular given node
    """
    def count_nodes(self, tree):
        #base case reached the terminal node so return 1
        if tree.type == 't':
            return 1
        #call this method again on the left and right chlid nodes
        else:
            return self.count_nodes(tree.n_left) + self.count_nodes(tree.n_right) + 1



"""
    * The Node class will either be a splitting or a terminal node
"""
class Node():

    """
        * If it is a spilling node, the column attribute will be column to split on, and the value will be the value in the column to split on
        * If it is a terminal node, then the value will be the terminal value to determine the class label
    """
    def __init__(self,type, n_column = '', n_value = 0.0, gain = 0.0 , n_groups = None ):
        self.n_column = n_column
        self.n_value = n_value
        self.n_groups = n_groups
        self.n_left = None
        self.n_right = None
        self.type = type
        self.gain = gain

if __name__ == "__main__":
    training_file_data = argv[1]
    training_file_label = argv[2]
    testing_file_data = argv[3]
    testing_file_label = argv[4]
    model = argv[5]
    training_set_percent = int(argv[6])

    if model == 'vanilla':
        vanilla_tree = Tree(training_file_data,training_file_label,testing_file_data,testing_file_label,model)
        data = vanilla_tree.load_data(training_file_data,training_file_label)
        test_data = vanilla_tree.load_data(testing_file_data,testing_file_label)

        max_depth = float('inf')
        sub_data = data[: (int(len(data) * int(training_set_percent) / 100))]

        tree = vanilla_tree.construct_tree(sub_data,max_depth)
        #print('Number of Nodes: ' + str(vanilla_tree.count_nodes(tree)))

        training_predicted_vals = vanilla_tree.predict(sub_data,tree)
        training_accuracy = vanilla_tree.analyze(training_predicted_vals)
        print("Training set accuracy: %.4f" % training_accuracy)

        testing_predicted_vals = vanilla_tree.predict(test_data ,tree)
        testing_accuracy = vanilla_tree.analyze(testing_predicted_vals)
        print("Testing set accuracy: %.4f" % testing_accuracy)

    elif model == 'depth':
        validation_set_percent = int(argv[7])
        max_depth = int(argv[8])

        depth_tree = Tree(training_file_data,training_file_label,testing_file_data,testing_file_label,model)
        data = depth_tree.load_data(training_file_data,training_file_label)
        test_data = depth_tree.load_data(testing_file_data,testing_file_label)


        sub_data = data[: (int(len(data) * int(training_set_percent) / 100))]
        v_sub_data = data[ (int(len(data) * int(100 - validation_set_percent) / 100)): ]

        tree= depth_tree.construct_tree(sub_data,max_depth)
        #print('Number of Nodes: ' + str(depth_tree.count_nodes(tree)))

        training_predicted_vals = depth_tree.predict(sub_data,tree)
        training_accuracy = depth_tree.analyze(training_predicted_vals)
        print("Training set accuracy: %.4f" % training_accuracy)

        validation_predicted_vals = depth_tree.predict(v_sub_data,tree)
        validation_accuracy = depth_tree.analyze(validation_predicted_vals)
        print("Validation set accuracy: %.4f" % validation_accuracy)

        testing_predicted_vals = depth_tree.predict(test_data ,tree)
        testing_accuracy = depth_tree.analyze(testing_predicted_vals)
        print("Testing set accuracy: %.4f" % testing_accuracy)

    elif model == 'min_split' :
        validation_set_percent = int(argv[7])
        min_split_samples = int(argv[8])

        min_split_tree = Tree(training_file_data,training_file_label,testing_file_data,testing_file_label,model)
        data = min_split_tree.load_data(training_file_data,training_file_label)
        test_data = min_split_tree.load_data(testing_file_data,testing_file_label)

        max_depth = float('inf')


        sub_data = data[: (int(len(data) * int(training_set_percent) / 100))]
        v_sub_data = data[ (int(len(data) * int(100 - validation_set_percent) / 100)): ]



        tree = min_split_tree.construct_tree(sub_data,max_depth, min_split_samples)
        #print('Number of Nodes: ' + str(min_split_tree.count_nodes(tree)))

        training_predicted_vals = min_split_tree.predict(sub_data,tree)
        training_accuracy = min_split_tree.analyze(training_predicted_vals)
        print("Training set accuracy: %.4f" % training_accuracy)

        validation_predicted_vals = min_split_tree.predict(v_sub_data,tree)
        validation_accuracy = min_split_tree.analyze(validation_predicted_vals)
        print("Validation set accuracy: %.4f" % validation_accuracy)

        testing_predicted_vals = min_split_tree.predict(test_data ,tree)
        testing_accuracy = min_split_tree.analyze(testing_predicted_vals)
        print("Testing set accuracy: %.4f" % testing_accuracy)

    elif model == 'prune':
        validation_set_percent = int(argv[7])

        prune_tree = Tree(training_file_data,training_file_label,testing_file_data,testing_file_label,model)
        data = prune_tree.load_data(training_file_data,training_file_label)
        test_data = prune_tree.load_data(testing_file_data,testing_file_label)

        max_depth = float('inf')

        sub_data = data[: (int(len(data) * int(training_set_percent) / 100))]
        v_sub_data = data[ (int(len(data) * int(100 - validation_set_percent) / 100)): ]



        tree = prune_tree.construct_tree(sub_data,max_depth, v_data = v_sub_data)
        #print('Number of Nodes: ' + str(prune_tree.count_nodes(tree)))

        training_predicted_vals = prune_tree.predict(sub_data,tree)
        training_accuracy = prune_tree.analyze(training_predicted_vals)
        print("Training set accuracy: %.4f" % training_accuracy)

        testing_predicted_vals = prune_tree.predict(test_data ,tree)
        testing_accuracy = prune_tree.analyze(testing_predicted_vals)
        print("Testing set accuracy: %.4f" % testing_accuracy)
