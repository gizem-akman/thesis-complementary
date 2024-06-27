1) clustered_points_and_centroids_with_distances.csv: contains optimal groups using the Kmean algorithm, and the furthest distance   from the group centroid. We used the KMeans implementation from the Python library sklearn.cluster.


2) non-optimal.csv: contains the furthest distance to the group centroid in the non optimal case, namely, the case illustrated in the video. We have 6 groups of 5 users, where group 1 corresponds to users labeled by 1-5 (in the video), group 2 consists of users labeled as 6-10...etc.

3)coordinates.csv: contains the points coordinates (from the simulation). Used as input in the previous Kmean algorithm and non-optimal.csv.

4)Simulation_final.cc: is the NS3 code. In the simulation, the users walk 2m/s and cars 15m/s

