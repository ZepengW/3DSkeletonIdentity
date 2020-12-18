import os
from readjsonApi import readjsonApi_p

videoJsonPath = '../../dataset/3dSkeletonGait/v2'

files = os.listdir(videoJsonPath)
for file in files:
    jsonFilePath = os.path.join(videoJsonPath,file)
    featureDirPath = os.path.join(videoJsonPath,'feature')
    if not os.path.exists(featureDirPath):
        os.mkdir(featureDirPath)
    if os.path.isdir(jsonFilePath) and ('_json' in file):
        featurePath = os.path.join(featureDirPath,file.split('.avi')[0]+'.txt')
        readjsonApi_p(jsonFilePath,featurePath)
