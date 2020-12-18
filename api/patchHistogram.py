import os
from histogramApi import genhist

featurePath = '../../dataset/3dSkeletonGait/v2/feature'
vectorPath = '../../dataset/3dSkeletonGait/v2/vector'
files = os.listdir(featurePath)
if not os.path.exists(vectorPath):
    os.mkdir(vectorPath)
for file in files:
    if not '.txt' in file:
        continue
    filePath = os.path.join(featurePath,file)
    genhist(filePath,'./',180,os.path.join(vectorPath,file))
