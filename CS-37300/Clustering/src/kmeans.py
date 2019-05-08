import numpy as np
import random
import math
import pandas as pd
import sys

class KMeans:

    def __init__(self, distance_choice):
        self.cluster_centers = None
        self.classes = {}
        self.dis_choice = 'euclidean'
        if distance_choice == 1:
            self.dis_choice = 'euclidean'
        elif distance_choice == 4:
            self.dis_choice = 'manhattan'

    def get_input(self,input_file,cluster_option):
        data = pd.read_csv(input_file, usecols = [3,4,6,7])
        data = data.dropna()
        if cluster_option == 2:
            data['reviewCount'] = np.log(data['reviewCount'])
            data['checkins'] = np.log(data['checkins'])
        elif cluster_option == 3:
            data['latitude'] = (data['latitude'] - (data['latitude'].mean()))  / (data['latitude'].std())
            data['longitude'] = (data['longitude'] - (data['longitude'].mean()))  / (data['longitude'].std())
            data['reviewCount'] = (data['reviewCount'] - (data['reviewCount'].mean()))  / (data['reviewCount'].std())
            data['checkins'] = (data['checkins'] - (data['checkins'].mean()))  / (data['checkins'].std())
        data = data.values
        return data

    def compute_distance(self, vector_1, vector_2, type = 'euclidean'):
        if type == 'euclidean':
            return np.linalg.norm(vector_1 - vector_2)
        elif type == 'manhattan':
            dis = 0
            for x in range(len(vector_1)):
                dis += math.fabs(vector_1[x] - vector_2[x])
            return dis

    def fit(self,data, K, cluster_option):
        choice_indices = np.random.choice(len(data),K)
        self.cluster_centers = np.array([data[i] for i in choice_indices])
        if cluster_option == 5:
            data = data[np.random.choice(data.shape[0], int(0.06 * len(data)), replace=False), :]
        optimal_centriod_dist = 0.0
        while(1):
            tot_centriod_dist = 0.0
            for y in range(K):
                self.classes[y] = []
            for vector in data:
                distances = [self.compute_distance(vector,centriod,self.dis_choice) for centriod in self.cluster_centers]
                tot_centriod_dist += min(distances)
                classification = distances.index(min(distances))
                self.classes[classification].append(vector)
            for classification in self.classes:
                if len(self.classes[classification]) != 0:
                    self.cluster_centers[classification] = np.average(self.classes[classification], axis = 0)
            if math.fabs(tot_centriod_dist - optimal_centriod_dist) < 0.0001:
                return self.cluster_centers, self.classes
            else:
                optimal_centriod_dist = tot_centriod_dist

    def score(self):
        score = 0.0
        for x in range(len(self.cluster_centers)):
            centriod = self.cluster_centers[x]
            cluster = self.classes[x]
            for point in cluster:
                score += self.compute_distance(point,centriod,self.dis_choice)**2
        return score

if __name__ == '__main__':
    if len(sys.argv) == 4:
        km = KMeans(int(sys.argv[3]))
        data = km.get_input(sys.argv[1],int(sys.argv[3]))
        centriods, classes = km.fit(data,int(sys.argv[2]),int(sys.argv[3]))
        score = km.score()
        print('WC-SSE=' + str(score))
        x = 1
        for centriod in centriods:
            print('Centriod' + str(x) + '=[' + str(centriod[0]) +',' + str(centriod[1])+ ',' + str(centriod[2])+ ',' + str(centriod[3])+ ']')
            x += 1
    else:
        print('\nUSAGE: python3 kmeans.py [path to input file] [k_num] [cluster option]\n')
