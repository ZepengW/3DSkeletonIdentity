import sys
import os
import pickle
from readjsonApi import get_feature_from_joints
from histogramApi import gen_vec_from_feature
from caculateDistance import get_sample_label



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
