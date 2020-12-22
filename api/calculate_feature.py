import sys
import os
import pickle
import numpy as np
from readjsonApi import get_feature_from_joints
from histogramApi import gen_vec_from_feature
from caculateDistance import get_sample_label
#¼ì²â
def get_label_score(skeletonFrameList,vectors):
    featureStr = get_feature_from_joints(skeletonFrameList)
    vecList = gen_vec_from_feature(featureStr)
    label, sim = get_sample_label(vecList, vectors)
    if np.isnan(sim):
        sim = 0
    return label, sim

#Â¼ÈëÊý¾Ý
def save_label(skeletonFrameList,datasetDir,label):
    featureStr = get_feature_from_joints(skeletonFrameList)
    vecList = gen_vec_from_feature(featureStr)
    files = os.listdir(datasetDir)
    videoIdList = [int(fName.split('v')[1].split('.')[0]) for fName in files if 'p'+label+'v' in fName]
    if len(videoIdList)==0:
        vId = 1
    else:
        vId = max(videoIdList)+1
    saveFileName = 'p'+label+'v'+str(vId)+'.txt'
    outputpath = os.path.join(datasetDir,saveFileName)
    pickle.dump(vecList, open(outputpath, 'wb'))
    return saveFileName

def get_joints_array(jointFilePath):
    file = open(jointFilePath)
    jointsFrameArray = []
    for line in file.readlines():
        if str(line)=="":
            continue
        jointsArray = line.split(',')
        jointsArray.__delitem__(len(jointsArray) - 1)
        jointsArray = [float(i) for i in jointsArray]
        jointsFrameArray.append(jointsArray)
    return jointsFrameArray

if __name__ == '__main__':
    jointsFrameArray = get_joints_array(sys.argv[1])
    featureStr = get_feature_from_joints(jointsFrameArray)
    vecList = gen_vec_from_feature(featureStr)
    #check dataset and caculate distance
    datasetDir = sys.argv[2]
    files = os.listdir(datasetDir)
    vectors = []
    for f in files:
        vector = pickle.load(open(os.path.join(datasetDir,f), 'rb'), encoding='iso-8859-1')
        vectors.append((vector, f.split('v')[0].split('p')[1]))

    label,sim = get_sample_label(vecList,vectors)
    print(str(label)+":"+str(sim))
    #output label and sim
    labelPath = sys.argv[3]
    f = open(labelPath,'w')
    f.writelines(str(label)+":"+str(sim))
    f.close()
