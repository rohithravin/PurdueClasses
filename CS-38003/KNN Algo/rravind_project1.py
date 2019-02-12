import pandas as pd
import numpy as np
import math
import operator
import random
import matplotlib.pyplot as plt

class MyKNN:

    def __init__(self):
        print('Welcome to KNN Algo')

    def plotData(self, parsedData, filename,title):
        plt.figure()
        plt.scatter(parsedData['feature_1'], parsedData['feature_2'], c=parsedData['class_label'])
        plt.xlabel('Feature 1')
        plt.ylabel('Feature 2')
        plt.title('Rohith Ravindranath' , fontsize = 10)
        plt.suptitle(title,  fontsize = 12)
        plt.savefig((str(filename) + '.png'))

    def plotF1Scores(self, lisx,lisy ,filename,title):
        plt.figure()
        plt.scatter(lisx, lisy)
        plt.xlabel('K-Value')
        plt.ylabel('F1 Score')
        plt.title('Rohith Ravindranath' , fontsize = 10)
        plt.suptitle(title,  fontsize = 12)
        plt.savefig((str(filename) + '.png'))

    def readData(self, filename):
        data = pd.read_csv(filename, header = None)
        data.columns = ['image_id' , 'class_label', 'feature_1', 'feature_2']
        return data

    def classify(self, k, parsedData, traininIds, testIds):
        predictions = []
        training_id_features = []
        for training_id in traininIds:
            training_feature_1  = float(parsedData.loc[(parsedData['image_id'] == training_id)]['feature_1'])
            training_feature_2  = float(parsedData.loc[(parsedData['image_id'] == training_id)]['feature_2'])
            training_id_features.append((training_feature_1,training_feature_2))
        for test_id in testIds:
            distance_tup = []
            class_labs = {}
            testing_feature_1  = float(parsedData.loc[(parsedData['image_id'] == test_id)]['feature_1'])
            testing_feature_2  = float(parsedData.loc[(parsedData['image_id'] == test_id)]['feature_2'])
            for x in range(len(training_id_features)):
                training_1 = training_id_features[x][0]
                training_2 = training_id_features[x][1]
                train_id = traininIds[x]
                dis = math.sqrt( math.pow((testing_feature_1 - training_1),2)  +  math.pow((testing_feature_2 -  training_2),2))
                distance_tup.append((train_id,dis))
            distance_tup.sort(key=operator.itemgetter(1))
            sorted_distance = distance_tup[:k]
            for id,dis in sorted_distance:
                class_label = int(parsedData.loc[(parsedData['image_id'] == id)]['class_label'])
                if class_label in class_labs.keys():
                    class_labs[class_label] = class_labs[class_label]  + 1
                else:
                    class_labs[class_label] = 1
            sorted_class_labels = sorted(class_labs.items(), key=operator.itemgetter(1))
            predictions.append(sorted_class_labels[0][0])
        return predictions

    def evaluate(self, parsedData, testIds, predictions):
        unique_values = list(set(predictions))
        unique_values.sort()
        hash = {}
        for i in range(len(unique_values)):
            v = unique_values[i]
            hash[v] = i
        confusion_matrix = np.zeros((len(unique_values),len(unique_values)) )
        f1_scores = []
        for counter in range(len(testIds)):
            id = testIds[counter]
            predicated_label = predictions[counter]
            actual_label = int(parsedData.loc[(parsedData['image_id'] == id)]['class_label'])
            confusion_matrix[hash[predicated_label]][hash[actual_label]] = confusion_matrix[hash[predicated_label]][hash[actual_label]] + 1
        sum_of_columns = confusion_matrix.sum(axis=1)
        sum_of_rows = confusion_matrix.sum(axis=0)
        sum_of_matrix = confusion_matrix.sum()
        for x in range(len(unique_values)):
            if confusion_matrix[x][x] != 0:
                tp = confusion_matrix[x][x]
                fn = sum_of_columns[x] - tp
                fp = sum_of_rows[x] - tp
                f1 = (2*tp) / (2*tp + fp + fn)
                f1_scores.append(f1)
        return confusion_matrix , (sum(f1_scores) / len(f1_scores))



print('----------------------------FULL DATASET--------------------------------')
#Using Full Dataset
f1_scores_list = []
k_list = [1,5,15,31]
knn = MyKNN()
parsedData = knn.readData('digits-embedding.csv')
knn.plotData(parsedData, 'Full_Dataset','Full Dataset Visualized Plot')
lis_ids = list(range(0,20000))
random.shuffle(lis_ids)
traininIds = lis_ids[:int(len(lis_ids)*0.8)]
testIds = lis_ids[int(len(lis_ids)*0.8):]
k_list.append(len(traininIds))
for k in k_list:
    print('Classifying Data for Full Dataset (k = ' + str(k) + ')...')
    predictions = knn.classify(k, parsedData, traininIds ,testIds)
    print('Data Classified for Full Dataset (k = ' + str(k) + ')...')
    print('Evluating Model for Full Dataset (k = ' + str(k) + ')...')
    confusion_matrix , average_f1 = knn.evaluate(parsedData,testIds,predictions)
    f1_scores_list.append(average_f1)
    print('Confusion Matrix for Full Dataset (k = ' + str(k) + '):' )
    print(str(confusion_matrix))
    print('Average F1 Score: for Full Dataset (k = ' + str(k) + '):' + str(average_f1) + '\n')
knn.plotF1Scores( k_list, f1_scores_list, 'Full_Dataset_F1_Scores' , 'Full Dataset K Value Plot')


#Using Digits 1 9 8 and 5
print('---------------------------DATASET 1 9 8 5------------------------------')
f1_scores_list = []
k_list = [1,5,15,31]
knn = MyKNN()
parsedData = knn.readData('digits-embedding.csv')
parsedData = parsedData.loc[parsedData['class_label'].isin([1,9,8,5])]
knn.plotData(parsedData, 'Dataset_1_9_8_5','Dataset 1 9 8 5 Visualized Plot')
ids = list(parsedData['image_id'])
random.shuffle(ids)
traininIds = ids[:int(len(ids)*0.8)]
testIds = ids[int(len(ids)*0.8):]
k_list.append(len(traininIds))
for k in k_list:
    print('Classifying Data for Dataset 1 9 8 5 (k = ' + str(k) + ')...')
    predictions = knn.classify(k, parsedData, traininIds ,testIds)
    print('Data Classified for Dataset 1 9 8 5 (k = ' + str(k) + ')...')
    print('Evluating Model for Dataset 1 9 8 5 (k = ' + str(k) + ')...')
    confusion_matrix , average_f1 = knn.evaluate(parsedData,testIds,predictions)
    f1_scores_list.append(average_f1)
    print('Confusion Matrix for Dataset 1 9 8 5 (k = ' + str(k) + '):' )
    print(str(confusion_matrix))
    print('Average F1 Score: for Dataset 1 9 8 5 (k = ' + str(k) + '):' + str(average_f1) + '\n')
knn.plotF1Scores( k_list, f1_scores_list, 'Dataset_1_9_8_5_F1_Scores' , 'Full Dataset 1 9 8 5 K Value Plot')



#Using Digits 1 9
print('----------------------------DATASET 1 9---------------------------------')
f1_scores_list = []
k_list = [1,5,15,31]
knn = MyKNN()
parsedData = knn.readData('digits-embedding.csv')
parsedData = parsedData.loc[parsedData['class_label'].isin([1,9])]
knn.plotData(parsedData, 'Dataset_1_9','Digits 1 9 Visualized Plot')
ids = list(parsedData['image_id'])
random.shuffle(ids)
traininIds = ids[:int(len(ids)*0.8)]
testIds = ids[int(len(ids)*0.8):]
k_list.append(len(traininIds))
for k in k_list:
    print('Classifying Data for Dataset 1 9 (k = ' + str(k) + ')...')
    predictions = knn.classify(k, parsedData, traininIds ,testIds)
    print('Data Classified for Dataset 1 9 (k = ' + str(k) + ').')
    print('Evluating Model for Dataset 1 9 (k = ' + str(k) + ')...')
    confusion_matrix , average_f1 = knn.evaluate(parsedData,testIds,predictions)
    f1_scores_list.append(average_f1)
    print('Confusion Matrix for Dataset 1 9 (k = ' + str(k) + '):' )
    print(str(confusion_matrix))
    print('Average F1 Score: for Dataset 1 9 (k = ' + str(k) + '):' + str(average_f1) + '\n')
knn.plotF1Scores(k_list,f1_scores_list,  'Dataset_1_9_F1_Scores' , 'Full Dataset 1 9 K Value Plot')
