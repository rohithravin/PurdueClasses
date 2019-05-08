import matplotlib.pyplot as plt
from kmeans import KMeans
import numpy as np

k_list = [3,6,9,12,24]
choice = 3
input_file = '../data/given/yelp.csv'


wcc_list = []

"""
for K in k_list:
    print(K)
    km = KMeans(choice)
    data = km.get_input(input_file,choice)
    centriods = km.fit(data,K,choice)
    score = km.score()
    print(score)
    wcc_list.append(score)

plt.figure()
plt.plot(k_list, wcc_list ,color='g')
plt.xlabel('K')
plt.ylabel('WC Scores')
plt.title('Rohith Ravindranath')
plt.suptitle('K vs. WC Score')
plt.show()

"""



km = KMeans(choice)
data = km.get_input(input_file,choice)
centriods, classes = km.fit(data,3,choice)


plt.figure()
for k in classes:
    print(k)
    g = np.array(classes[k])
    plt.scatter(g[:,0], g[:,1])
plt.scatter(centriods[:,0], centriods[:,1])
plt.xlabel('latitude')
plt.ylabel('longitude')
plt.title('Rohith Ravindranath')
plt.suptitle('latitude vs. longitude')
plt.show()


plt.figure()
for k in classes:
    print(k)
    g = np.array(classes[k])
    plt.scatter(g[:,2], g[:,3])
plt.scatter(centriods[:,2], centriods[:,3])
plt.xlabel('reviewCount')
plt.ylabel('checkins')
plt.title('Rohith Ravindranath')
plt.suptitle('latitude vs. longitude')
plt.show()
