# -- coding:utf-8 --
import json
import os
import numpy as np
import math
import sys
import setting as st

def computeRJP(s_p):
    feature=[]
    x=s_p[1*3]
    y=s_p[1*3+1]

    for i in range(st._SKNUM+1):
        if i==1:  #skip 1 node
            continue
        x1=s_p[i*3]-x
        y1=s_p[i*3+1]-y
        r1=math.atan2(y1, x1)
        feature.append(x1)
        feature.append(y1)
        feature.append(r1)

    feature=np.array(feature)
    #feature=feature*1.0/(np.sqrt(np.sum(feature**2)))
    feature=feature*1.0

    #H=(s_p[8*3+1]+s_p[9*3+1])/2-s_p[1*3+1]

    for i in range(int(len(feature)/3)):
        feature[i*3]=feature[i*3]
        feature[i*3+1]=feature[i*3+1]
        feature[i*3+2]=feature[i*3+2]/(2*np.pi)+0.5

    return feature


def computeAngle(s_p):
    feature=[]
    total=0
    totalangle=0

    limb12=np.array([s_p[2*3]-s_p[1*3],s_p[2*3+1]-s_p[1*3+1]])
    limb10=np.array([s_p[0*3]-s_p[1*3],s_p[0*3+1]-s_p[1*3+1]])
    lx=np.sqrt(limb12.dot(limb12))
    ly=np.sqrt(limb10.dot(limb10))
    cos_angle=limb12.dot(limb10)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    #totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    #total+=lx**2 #12
    total+=ly**2 #10

    #limb12=np.array([s_p[2*3]-s_p[1*3],s_p[2*3+1]-s_p[1*3+1]])
    limb15=np.array([s_p[5*3]-s_p[1*3],s_p[5*3+1]-s_p[1*3+1]])
    lx=np.sqrt(limb12.dot(limb12))
    ly=np.sqrt(limb15.dot(limb15))

    cos_angle=limb12.dot(limb15)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    #totalangle+=angle**2
    #12-15

    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=lx**2 #12
    total+=ly**2 #15
    #print(limb12,limb15,lx,ly,angle*360/2/np.pi)

    limb18=np.array([s_p[8*3]-s_p[1*3],s_p[8*3+1]-s_p[1*3+1]])
    #12-18
    lx=np.sqrt(limb12.dot(limb12))
    ly=np.sqrt(limb18.dot(limb18))
    cos_angle=limb12.dot(limb18)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2

    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #18
    #18-111
    limb111=np.array([s_p[11*3]-s_p[1*3],s_p[11*3+1]-s_p[1*3+1]])
    lx=np.sqrt(limb18.dot(limb18))
    ly=np.sqrt(limb111.dot(limb111))
    cos_angle=limb18.dot(limb111)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #111
    #15-111
    lx=np.sqrt(limb15.dot(limb15))
    ly=np.sqrt(limb111.dot(limb111))
    cos_angle=limb15.dot(limb111)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    #21-23
    limb21=np.array([s_p[1*3]-s_p[2*3],s_p[1*3+1]-s_p[2*3+1]])
    limb23=np.array([s_p[3*3]-s_p[2*3],s_p[3*3+1]-s_p[2*3+1]])
    lx=np.sqrt(limb21.dot(limb21))
    ly=np.sqrt(limb23.dot(limb23))
    cos_angle=limb21.dot(limb23)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #23
    #32-34
    limb32=np.array([s_p[2*3]-s_p[3*3],s_p[2*3+1]-s_p[3*3+1]])
    limb34=np.array([s_p[4*3]-s_p[3*3],s_p[4*3+1]-s_p[3*3+1]])
    lx=np.sqrt(limb32.dot(limb32))
    ly=np.sqrt(limb34.dot(limb34))
    cos_angle=limb32.dot(limb34)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #34
    #51-56
    limb51=np.array([s_p[1*3]-s_p[5*3],s_p[1*3+1]-s_p[5*3+1]])
    limb56=np.array([s_p[6*3]-s_p[5*3],s_p[6*3+1]-s_p[5*3+1]])
    lx=np.sqrt(limb51.dot(limb51))
    ly=np.sqrt(limb56.dot(limb56))
    cos_angle=limb51.dot(limb56)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #56
    #65-67
    limb65=np.array([s_p[5*3]-s_p[6*3],s_p[5*3+1]-s_p[6*3+1]])
    limb67=np.array([s_p[7*3]-s_p[6*3],s_p[7*3+1]-s_p[6*3+1]])
    lx=np.sqrt(limb65.dot(limb65))
    ly=np.sqrt(limb67.dot(limb67))
    cos_angle=limb65.dot(limb67)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #57
    #81-89
    limb81=np.array([s_p[1*3]-s_p[8*3],s_p[1*3+1]-s_p[8*3+1]])
    limb89=np.array([s_p[9*3]-s_p[8*3],s_p[9*3+1]-s_p[8*3+1]])
    lx=np.sqrt(limb81.dot(limb81))
    ly=np.sqrt(limb89.dot(limb89))
    cos_angle=limb81.dot(limb89)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #89
    #98-910
    limb98=np.array([s_p[8*3]-s_p[9*3],s_p[8*3+1]-s_p[9*3+1]])
    limb910=np.array([s_p[10*3]-s_p[9*3],s_p[10*3+1]-s_p[9*3+1]])
    lx=np.sqrt(limb98.dot(limb98))
    ly=np.sqrt(limb910.dot(limb910))
    cos_angle=limb98.dot(limb910)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #910
    #111-1112
    limb111=np.array([s_p[1*3]-s_p[11*3],s_p[1*3+1]-s_p[11*3+1]])
    limb1112=np.array([s_p[12*3]-s_p[11*3],s_p[12*3+1]-s_p[11*3+1]])
    lx=np.sqrt(limb111.dot(limb111))
    ly=np.sqrt(limb1112.dot(limb1112))
    cos_angle=limb111.dot(limb1112)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #1112
    #1211-1213
    limb1211=np.array([s_p[11*3]-s_p[12*3],s_p[11*3+1]-s_p[12*3+1]])
    limb1213=np.array([s_p[13*3]-s_p[12*3],s_p[13*3+1]-s_p[12*3+1]])
    lx=np.sqrt(limb1211.dot(limb1211))
    ly=np.sqrt(limb1213.dot(limb1213))
    cos_angle=limb1211.dot(limb1213)/(lx*ly+0.001)
    if cos_angle<-1 or cos_angle>1:
        cos_angle=0
    angle=np.arccos(cos_angle)
    totalangle+=angle**2
    feature.append(lx)
    feature.append(ly)
    feature.append(angle)
    total+=ly**2 #1213

    feature=np.array(feature)
    feature=feature*1.0
    total=np.sqrt(total)

    #totalangle=np.sqrt(totalangle)
    for i in range(int(len(feature)/3)):
        feature[i*3]=feature[i*3]/total
        feature[i*3+1]=feature[i*3+1]/total
        feature[i*3+2]=feature[i*3+2]/np.pi

    return feature


def computeGlobal(s_p,pres_p):
    for i in range(st._SKNUM2):
        s_p[i*3+2]=0
        pres_p[i*3+2]=0
    return np.array(s_p)-np.array(pres_p)

def computeLocal(s_p,pres_p):

    x=s_p[1*3]
    y=s_p[1*3+1]

    for i in range(st._SKNUM3):
        s_p[i*3]=s_p[i*3]-x
        s_p[i*3+1]=s_p[i*3+1]-y
        s_p[i*3+2]=0

    x=pres_p[1*3]
    y=pres_p[1*3+1]

    for i in range(st._SKNUM3):
        pres_p[i*3]=pres_p[i*3]-x
        pres_p[i*3+1]=pres_p[i*3+1]-y
        pres_p[i*3+2]=0
    return np.array(s_p)-np.array(pres_p)


def get_feature_from_joints(jointsFramesList):
    # intialization
    outStr = str(len(jointsFramesList)) + "\n"
    pres_p = []
    pres_p1 = []
    pre_feature1 = []
    pre_feature2 = []
    traj_ind = 0
    group_ind = 0
    traj = [[[] for j in range(14)] for i in range(int(len(jointsFramesList) / st._TEMPOLEN) + 1)]
    count = 0
    for s_p in jointsFramesList:
        ss = np.array(s_p)
        ss = np.where(ss == 0)
        if len(ss[0]) > 30:
            continue
        count += 1

        feature = computeRJP(s_p)
        feature1 = computeAngle(s_p)
        s_p = np.array(s_p)

        if pre_feature1 == [] or pre_feature2 == []:
            pre_feature1 = feature.copy()
            pre_feature2 = feature1.copy()
            pres_p = s_p
            continue

        feature2 = computeGlobal(s_p, pres_p)
        feature3 = computeLocal(s_p, pres_p)

        for i in range(14):
            # print(s_p[i*3],s_p[i*3+1],pres_p[i*3],pres_p[i*3+1])
            x = s_p[i * 3] - pres_p[i * 3]
            y = s_p[i * 3 + 1] - pres_p[i * 3 + 1]
            traj[group_ind][i].append(np.sqrt(pow(x, 2) + pow(y, 2)))
            traj[group_ind][i].append(math.atan2(y, x))

        pre_feature1 = feature.copy()
        pre_feature2 = feature1.copy()
        pres_p = s_p.copy()
        # print(feature,feature1,feature2,feature3)
        feature = [str(round(x, 4)) for x in feature]
        feature1 = [str(round(x, 4)) for x in feature1]
        feature2 = [str(round(x, 4)) for x in feature2]
        feature3 = [str(round(x, 4)) for x in feature3]
        # print(len(feature),len(feature1),len(feature2),len(feature3))
        feature.extend(feature1)
        feature.extend(feature2)
        feature.extend(feature3)
        # print(len(feature))
        str_convert = ','.join(feature)
        outStr += str_convert + "\n"
        traj_ind += 1
        # print(traj)
        if traj_ind > st._TEMPOLEN - 1:
            traj_ind = 0
            group_ind += 1

    for i in range(int(len(jointsFramesList) / st._TEMPOLEN)):
        for j in range(14):
            tmp = traj[i][j]
            tmp = [str(x) for x in tmp]
            tmp1 = ','.join(tmp)
            outStr += tmp1 + "\n"
    return outStr

def readjsonApi_p(folderpath, outpath):
    #input:
    #folderpath: a folder path containing json file paths, each json contains skeletons for a frame
    #outpath: a .txt file contains features for a gait video
    #folderpath='./video1  (including 1.json, 2.json...)'    outpath="./video1.txt"
    #output: a txt file
    #first line is the frame number
    #LGSD features:
    #static lines:17*3+13*3+18*3+18*3=198
    #dynamic lines:8*2=16


    parents = os.listdir(folderpath)
    parents.sort()
    #list all files in folderr
    ffile=open(outpath,"w")
    ffile.write(str(len(parents))+"\n")
    #intialization
    pres_p=[]
    pres_p1=[]
    pre_feature1=[]
    pre_feature2=[]
    traj_ind=0
    group_ind=0
    traj=[[[] for j in range(14)] for i in range(int(len(parents)/st._TEMPOLEN)+1)]
    count=0

    for parent in parents:
        child = os.path.join(folderpath,parent)

        with open(child,"r") as f:

            s = json.load(f)
            if s["people"]==[]:
                print("empty")
                continue
            elif s["people"][0]==[]:
                print("empty")
                continue
            s_p=s["people"][0]["pose_keypoints_2d"]
            ss=np.array(s_p)
            ss=np.where(ss==0)
            if len(ss[0])>30:
                continue

            count+=1

        feature=computeRJP(s_p)
        feature1=computeAngle(s_p)
        s_p=np.array(s_p)

        if pre_feature1==[] or pre_feature2==[]:
            pre_feature1=feature.copy()
            pre_feature2=feature1.copy()
            pres_p=s_p
            continue

        feature2=computeGlobal(s_p,pres_p)
        feature3=computeLocal(s_p,pres_p)

        for i in range(14):
            #print(s_p[i*3],s_p[i*3+1],pres_p[i*3],pres_p[i*3+1])
            x=s_p[i*3]-pres_p[i*3]
            y=s_p[i*3+1]-pres_p[i*3+1]
            traj[group_ind][i].append(np.sqrt(pow(x,2)+pow(y,2)))
            traj[group_ind][i].append(math.atan2(y, x))

        pre_feature1=feature.copy()
        pre_feature2=feature1.copy()
        pres_p=s_p.copy()
        #print(feature,feature1,feature2,feature3)
        feature= [str(round(x,4)) for x in feature]
        feature1=[str(round(x,4)) for x in feature1]
        feature2= [str(round(x,4)) for x in feature2]
        feature3=[str(round(x,4)) for x in feature3]
        #print(len(feature),len(feature1),len(feature2),len(feature3))
        feature.extend(feature1)
        feature.extend(feature2)
        feature.extend(feature3)
        #print(len(feature))
        str_convert = ','.join(feature)
        ffile.write(str_convert+"\n")
        traj_ind+=1
        #print(traj)
        if traj_ind>st._TEMPOLEN-1:
            traj_ind=0
            group_ind+=1

    print(count)
    for i in range(int(len(parents)/st._TEMPOLEN)):
        for j in range(14):
            tmp=traj[i][j]
            tmp=[str(x) for x in tmp]
            tmp1=','.join(tmp)
            ffile.write(tmp1+"\n")
    ffile.close()
    #print(ere)

if __name__ == '__main__':
     #python readjsonApi.py /home/bird/xuke/gait/output_0510(1-40)/001-bg-01-000 ./video1.txt
    print(sys.argv)
    readjsonApi_p(sys.argv[1],sys.argv[2])