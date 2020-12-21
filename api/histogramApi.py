# -- coding:utf-8 --
import json
import cv2
import os
import numpy as np
import math
from scipy.cluster.vq import vq, kmeans, whiten
#from sklearn.cluster import KMeans
import pickle
from matplotlib import pyplot as plt
from sklearn import svm
from sklearn.decomposition import PCA

from mpl_toolkits.mplot3d import Axes3D
#读入center文件,生成0-62的直方图，每段1个向量,保存在histogram
import setting as st
import sys
#os.system('mkdir ./histogram')
#os.system('rm ./histogram/*')


def standardization(data):
    mu = np.mean(data, axis=0)
    sigma = np.std(data, axis=0)
    return (data - mu) / sigma

def cos_sim(vector_a, vector_b):
    """
    计算两个向量之间的余弦相似度
    :param vector_a: 向量 a
    :param vector_b: 向量 b
    :return: sim
    """
    vector_a = np.mat(vector_a)
    vector_b = np.mat(vector_b)
    num = float(vector_a * vector_b.T)
    denom = np.linalg.norm(vector_a) * np.linalg.norm(vector_b)
    cos = num / denom
    sim = 0.5 + 0.5 * cos
    return sim

def loadcenter(center_path,_MIDDLE):
#center_path='./center/'
#_MIDDLE=st._MIDDLE
	center_point=[]
	center_angle=[]
	center_delta_point=[]
	center_delta_angle=[]
	center_traj=[]
	#center_skele=[]

	#for i in range(st._SKNUM1):
	cpath=center_path+str(_MIDDLE)+'_vars_point'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_point.append(codebook)
	center_point=np.array(center_point)
	center_point=np.squeeze(center_point)
	print(center_point.shape)

	#for i in range(st._SKNUM):
	cpath=center_path+str(_MIDDLE)+'_vars_angle'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_angle.append(codebook)

	center_angle=np.array(center_angle)
	center_angle=np.squeeze(center_angle)
	print(center_angle.shape)

	#for i in range(st._SKNUM1):
	cpath=center_path+str(_MIDDLE)+'_vars_delta_point'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_delta_point.append(codebook)
	center_delta_point=np.array(center_delta_point)
	center_delta_point=np.squeeze(center_delta_point)
	print(center_delta_point.shape)

	#for i in range(st._SKNUM):
	cpath=center_path+str(_MIDDLE)+'_vars_delta_angle'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_delta_angle.append(codebook)
	center_delta_angle=np.array(center_delta_angle)
	center_delta_angle=np.squeeze(center_delta_angle)
	print(center_delta_angle.shape)

	#for i in range(1):
	cpath=center_path+str(_MIDDLE)+'_vars_traj.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_traj.append(codebook)

	center_traj=np.array(center_traj)
	center_traj=np.squeeze(center_traj)
	print(center_traj.shape)
	return center_point,center_angle,center_delta_point,center_delta_angle,center_traj

def genhist(featurepath,center_path,_MIDDLE,outputpath):
	#input:
	#outputpath:dump txt file, histogram for a video
	center_point,center_angle,center_delta_point,center_delta_angle,center_traj=loadcenter(center_path,_MIDDLE)

	histogram_final=[]
	child=featurepath
		#child = os.path.join(path,parent)
		#if os.path.isfile(child):
	vars_point=[]
	vars_angle=[]
	vars_delta_point=[]
	vars_delta_angle=[]
	vars_traj=[]
	#vars_skele=[[] for i in range(1)]
#聚类中心设置
	histogram_point=np.zeros((1,st._CENTER1))
	histogram_angle=np.zeros((1,st._CENTER1))
	histogram_delta_point=np.zeros((1,st._CENTER1))
	histogram_delta_angle=np.zeros((1,st._CENTER1))
	histogram_traj=np.zeros((1,st._CENTER2))

	child=child.replace('\n','')
	if os.path.exists(child):

		f=open(child,'r')
		lines=f.readlines()
		f.close()
		num1=0
		num2=0

		for line in lines:
			line=line.replace('\n','')
			feature=line.split(',')
			#print(len(feature))
			if len(feature)==st._SKNUM*3+st._SKNUM1*3+st._SKNUM2*3+st._SKNUM3*3:
				num1+=1
				feature=[float(x) for x in feature]
				#vars_skele[0].append(feature)
				for i in range(0,st._SKNUM):
					vars_point.append(feature[i*3:i*3+3])
				for i in range(st._SKNUM,st._SKNUM+st._SKNUM1):
					vars_angle.append(feature[i*3:i*3+3])
				for i in range(st._SKNUM+st._SKNUM1,st._SKNUM+st._SKNUM1+st._SKNUM2):
					vars_delta_point.append(feature[i*3:i*3+3])
				for i in range(st._SKNUM+st._SKNUM1+st._SKNUM2,st._SKNUM+st._SKNUM1+st._SKNUM2+st._SKNUM3):
					vars_delta_angle.append(feature[i*3:i*3+3])

			if len(feature)==st._TEMPOLEN*2:

				feature=[float(x) for x in feature]
				vars_traj.append(feature)
				num2+=1


		vars_point=np.array(vars_point)
		vars_angle=np.array(vars_angle)
		vars_delta_point=np.array(vars_delta_point)
		vars_delta_angle=np.array(vars_delta_angle)
		vars_traj=np.array(vars_traj)

		thres=10
		X= vars_point
		print(X.shape,center_point.shape)
		label,dist=vq(X,center_point)
		m=-1
		for j in label:
			#print(dist[j])
			m+=1
			if dist[m]<thres:
				histogram_point[0][j]+=1

		X=vars_delta_point
		label,dist=vq(X,center_delta_point)
		m=-1
		for j in label:
			m+=1
			if dist[m]<thres:
				histogram_delta_point[0][j]+=1

		#for i in range(0,st._SKNUM):
		X= vars_angle
		label,dist=vq(X,center_angle)
		m=-1
		for j in label:
			m+=1
			if dist[m]<thres:
				histogram_angle[0][j]+=1

		X= vars_delta_angle
		label,dist=vq(X,center_delta_angle)
		m=-1
		for j in label:
			m+=1
			if dist[m]<thres:
				histogram_delta_angle[0][j]+=1

		m=-1
		X= vars_traj
		label,dist=vq(X,center_traj)
		#print (dist)
		for j in label:
			m+=1
			if dist[m]<30:
				histogram_traj[0][j]+=1
		#print(df)
		print(np.sum(histogram_point[0]),np.sum(histogram_angle[0]),np.sum(histogram_delta_point[0]),np.sum(histogram_delta_angle[0]),np.sum(histogram_traj[0]))

		histogram_point[0]=standardization(histogram_point[0])
		histogram_angle[0]=standardization(histogram_angle[0])
		histogram_delta_point[0]=standardization(histogram_delta_point[0])
		histogram_delta_angle[0]=standardization(histogram_delta_angle[0])
		histogram_traj[0]=standardization(histogram_traj[0])

	histogram_point=np.ravel(histogram_point)
	histogram_angle=np.ravel(histogram_angle)
	histogram_delta_point=np.ravel(histogram_delta_point)
	histogram_delta_angle=np.ravel(histogram_delta_angle)
	histogram_traj=np.ravel(histogram_traj)

	temp=np.hstack((histogram_point,histogram_angle))
	#temp=np.hstack((temp,histogram_delta_point))
	#temp=np.hstack((temp,histogram_delta_angle))
	#temp=np.hstack((temp,histogram_traj))

	histogram_final.append(temp)
	print(histogram_final,np.array(histogram_final).shape)
	pickle.dump(histogram_final,open(outputpath, 'wb') )
	#'./histogram/'+name+'_histogram_final_0_'+str(_MIDDLE)+'_train.txt'

if __name__ == '__main__':
     #python histogramApi.py ./video1.txt,./ 180 ./histogram.txt
    print(sys.argv)
    genhist(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])







# -- coding:utf-8 --
import json
import cv2
import os
import numpy as np
import math
from scipy.cluster.vq import vq, kmeans, whiten
#from sklearn.cluster import KMeans
import pickle
from matplotlib import pyplot as plt
from sklearn import svm
from sklearn.decomposition import PCA

from mpl_toolkits.mplot3d import Axes3D
#读入center文件,生成0-62的直方图，每段1个向量,保存在histogram
import setting as st
import sys
#os.system('mkdir ./histogram')
#os.system('rm ./histogram/*')


def standardization(data):
    mu = np.mean(data, axis=0)
    sigma = np.std(data, axis=0)
    return (data - mu) / sigma

def cos_sim(vector_a, vector_b):
    """
    计算两个向量之间的余弦相似度
    :param vector_a: 向量 a
    :param vector_b: 向量 b
    :return: sim
    """
    vector_a = np.mat(vector_a)
    vector_b = np.mat(vector_b)
    num = float(vector_a * vector_b.T)
    denom = np.linalg.norm(vector_a) * np.linalg.norm(vector_b)
    cos = num / denom
    sim = 0.5 + 0.5 * cos
    return sim

def loadcenter(center_path,_MIDDLE):
#center_path='./center/'
#_MIDDLE=st._MIDDLE
	center_point=[]
	center_angle=[]
	center_delta_point=[]
	center_delta_angle=[]
	center_traj=[]
	#center_skele=[]

	#for i in range(st._SKNUM1):
	cpath=center_path+str(_MIDDLE)+'_vars_point'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_point.append(codebook)
	center_point=np.array(center_point)
	center_point=np.squeeze(center_point)
	print(center_point.shape)

	#for i in range(st._SKNUM):
	cpath=center_path+str(_MIDDLE)+'_vars_angle'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_angle.append(codebook)

	center_angle=np.array(center_angle)
	center_angle=np.squeeze(center_angle)
	print(center_angle.shape)

	#for i in range(st._SKNUM1):
	cpath=center_path+str(_MIDDLE)+'_vars_delta_point'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_delta_point.append(codebook)
	center_delta_point=np.array(center_delta_point)
	center_delta_point=np.squeeze(center_delta_point)
	print(center_delta_point.shape)

	#for i in range(st._SKNUM):
	cpath=center_path+str(_MIDDLE)+'_vars_delta_angle'+'.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_delta_angle.append(codebook)
	center_delta_angle=np.array(center_delta_angle)
	center_delta_angle=np.squeeze(center_delta_angle)
	print(center_delta_angle.shape)

	#for i in range(1):
	cpath=center_path+str(_MIDDLE)+'_vars_traj.txt'
	with open(cpath,'rb') as f:  # Python 3: open(..., 'rb')
		codebook, distortion = pickle.load(f,encoding='iso-8859-1')
		center_traj.append(codebook)

	center_traj=np.array(center_traj)
	center_traj=np.squeeze(center_traj)
	print(center_traj.shape)
	return center_point,center_angle,center_delta_point,center_delta_angle,center_traj

def gen_vec_from_feature(featureStr):
	center_path="./"
	_MIDDLE=180
	# input: featureStr (same as file)
	center_point, center_angle, center_delta_point, center_delta_angle, center_traj = loadcenter(center_path, _MIDDLE)

	histogram_final = []
	# child = os.path.join(path,parent)
	# if os.path.isfile(child):
	vars_point = []
	vars_angle = []
	vars_delta_point = []
	vars_delta_angle = []
	vars_traj = []
	# vars_skele=[[] for i in range(1)]
	# 聚类中心设置
	histogram_point = np.zeros((1, st._CENTER1))
	histogram_angle = np.zeros((1, st._CENTER1))
	histogram_delta_point = np.zeros((1, st._CENTER1))
	histogram_delta_angle = np.zeros((1, st._CENTER1))
	histogram_traj = np.zeros((1, st._CENTER2))

	if ""!=featureStr:

		f = featureStr
		lines = f.split('\n')
		num1 = 0
		num2 = 0

		for line in lines:
			line = line.replace('\n', '')
			feature = line.split(',')
			# print(len(feature))
			if len(feature) == st._SKNUM * 3 + st._SKNUM1 * 3 + st._SKNUM2 * 3 + st._SKNUM3 * 3:
				num1 += 1
				feature = [float(x) for x in feature]
				# vars_skele[0].append(feature)
				for i in range(0, st._SKNUM):
					vars_point.append(feature[i * 3:i * 3 + 3])
				for i in range(st._SKNUM, st._SKNUM + st._SKNUM1):
					vars_angle.append(feature[i * 3:i * 3 + 3])
				for i in range(st._SKNUM + st._SKNUM1, st._SKNUM + st._SKNUM1 + st._SKNUM2):
					vars_delta_point.append(feature[i * 3:i * 3 + 3])
				for i in range(st._SKNUM + st._SKNUM1 + st._SKNUM2, st._SKNUM + st._SKNUM1 + st._SKNUM2 + st._SKNUM3):
					vars_delta_angle.append(feature[i * 3:i * 3 + 3])

			if len(feature) == st._TEMPOLEN * 2:
				feature = [float(x) for x in feature]
				vars_traj.append(feature)
				num2 += 1

		vars_point = np.array(vars_point)
		vars_angle = np.array(vars_angle)
		vars_delta_point = np.array(vars_delta_point)
		vars_delta_angle = np.array(vars_delta_angle)
		vars_traj = np.array(vars_traj)

		thres = 10
		X = vars_point
		print(X.shape, center_point.shape)
		label, dist = vq(X, center_point)
		m = -1
		for j in label:
			# print(dist[j])
			m += 1
			if dist[m] < thres:
				histogram_point[0][j] += 1

		X = vars_delta_point
		label, dist = vq(X, center_delta_point)
		m = -1
		for j in label:
			m += 1
			if dist[m] < thres:
				histogram_delta_point[0][j] += 1

		# for i in range(0,st._SKNUM):
		X = vars_angle
		label, dist = vq(X, center_angle)
		m = -1
		for j in label:
			m += 1
			if dist[m] < thres:
				histogram_angle[0][j] += 1

		X = vars_delta_angle
		label, dist = vq(X, center_delta_angle)
		m = -1
		for j in label:
			m += 1
			if dist[m] < thres:
				histogram_delta_angle[0][j] += 1

		m = -1
		X = vars_traj
		label, dist = vq(X, center_traj)
		# print (dist)
		for j in label:
			m += 1
			if dist[m] < 30:
				histogram_traj[0][j] += 1
		# print(df)
		print(np.sum(histogram_point[0]), np.sum(histogram_angle[0]), np.sum(histogram_delta_point[0]),
			  np.sum(histogram_delta_angle[0]), np.sum(histogram_traj[0]))

		histogram_point[0] = standardization(histogram_point[0])
		histogram_angle[0] = standardization(histogram_angle[0])
		histogram_delta_point[0] = standardization(histogram_delta_point[0])
		histogram_delta_angle[0] = standardization(histogram_delta_angle[0])
		histogram_traj[0] = standardization(histogram_traj[0])

	histogram_point = np.ravel(histogram_point)
	histogram_angle = np.ravel(histogram_angle)
	histogram_delta_point = np.ravel(histogram_delta_point)
	histogram_delta_angle = np.ravel(histogram_delta_angle)
	histogram_traj = np.ravel(histogram_traj)

	temp = np.hstack((histogram_point, histogram_angle))
	# temp=np.hstack((temp,histogram_delta_point))
	# temp=np.hstack((temp,histogram_delta_angle))
	# temp=np.hstack((temp,histogram_traj))

	histogram_final.append(temp)
	print(histogram_final, np.array(histogram_final).shape)

	return histogram_final



# './histogram/'+name+'_histogram_final_0_'+str(_MIDDLE)+'_train.txt'
def genhist(featurepath,center_path,_MIDDLE,outputpath):
	#input:
	#outputpath:dump txt file, histogram for a video
	center_point,center_angle,center_delta_point,center_delta_angle,center_traj=loadcenter(center_path,_MIDDLE)

	histogram_final=[]
	child=featurepath
		#child = os.path.join(path,parent)
		#if os.path.isfile(child):
	vars_point=[]
	vars_angle=[]
	vars_delta_point=[]
	vars_delta_angle=[]
	vars_traj=[]
	#vars_skele=[[] for i in range(1)]
#聚类中心设置
	histogram_point=np.zeros((1,st._CENTER1))
	histogram_angle=np.zeros((1,st._CENTER1))
	histogram_delta_point=np.zeros((1,st._CENTER1))
	histogram_delta_angle=np.zeros((1,st._CENTER1))
	histogram_traj=np.zeros((1,st._CENTER2))

	child=child.replace('\n','')
	if os.path.exists(child):

		f=open(child,'r')
		lines=f.readlines()
		f.close()
		num1=0
		num2=0

		for line in lines:
			line=line.replace('\n','')
			feature=line.split(',')
			#print(len(feature))
			if len(feature)==st._SKNUM*3+st._SKNUM1*3+st._SKNUM2*3+st._SKNUM3*3:
				num1+=1
				feature=[float(x) for x in feature]
				#vars_skele[0].append(feature)
				for i in range(0,st._SKNUM):
					vars_point.append(feature[i*3:i*3+3])
				for i in range(st._SKNUM,st._SKNUM+st._SKNUM1):
					vars_angle.append(feature[i*3:i*3+3])
				for i in range(st._SKNUM+st._SKNUM1,st._SKNUM+st._SKNUM1+st._SKNUM2):
					vars_delta_point.append(feature[i*3:i*3+3])
				for i in range(st._SKNUM+st._SKNUM1+st._SKNUM2,st._SKNUM+st._SKNUM1+st._SKNUM2+st._SKNUM3):
					vars_delta_angle.append(feature[i*3:i*3+3])

			if len(feature)==st._TEMPOLEN*2:

				feature=[float(x) for x in feature]
				vars_traj.append(feature)
				num2+=1


		vars_point=np.array(vars_point)
		vars_angle=np.array(vars_angle)
		vars_delta_point=np.array(vars_delta_point)
		vars_delta_angle=np.array(vars_delta_angle)
		vars_traj=np.array(vars_traj)

		thres=10
		X= vars_point
		print(X.shape,center_point.shape)
		label,dist=vq(X,center_point)
		m=-1
		for j in label:
			#print(dist[j])
			m+=1
			if dist[m]<thres:
				histogram_point[0][j]+=1

		X=vars_delta_point
		label,dist=vq(X,center_delta_point)
		m=-1
		for j in label:
			m+=1
			if dist[m]<thres:
				histogram_delta_point[0][j]+=1

		#for i in range(0,st._SKNUM):
		X= vars_angle
		label,dist=vq(X,center_angle)
		m=-1
		for j in label:
			m+=1
			if dist[m]<thres:
				histogram_angle[0][j]+=1

		X= vars_delta_angle
		label,dist=vq(X,center_delta_angle)
		m=-1
		for j in label:
			m+=1
			if dist[m]<thres:
				histogram_delta_angle[0][j]+=1

		m=-1
		X= vars_traj
		label,dist=vq(X,center_traj)
		#print (dist)
		for j in label:
			m+=1
			if dist[m]<30:
				histogram_traj[0][j]+=1
		#print(df)
		print(np.sum(histogram_point[0]),np.sum(histogram_angle[0]),np.sum(histogram_delta_point[0]),np.sum(histogram_delta_angle[0]),np.sum(histogram_traj[0]))

		histogram_point[0]=standardization(histogram_point[0])
		histogram_angle[0]=standardization(histogram_angle[0])
		histogram_delta_point[0]=standardization(histogram_delta_point[0])
		histogram_delta_angle[0]=standardization(histogram_delta_angle[0])
		histogram_traj[0]=standardization(histogram_traj[0])

	histogram_point=np.ravel(histogram_point)
	histogram_angle=np.ravel(histogram_angle)
	histogram_delta_point=np.ravel(histogram_delta_point)
	histogram_delta_angle=np.ravel(histogram_delta_angle)
	histogram_traj=np.ravel(histogram_traj)

	temp=np.hstack((histogram_point,histogram_angle))
	#temp=np.hstack((temp,histogram_delta_point))
	#temp=np.hstack((temp,histogram_delta_angle))
	#temp=np.hstack((temp,histogram_traj))

	histogram_final.append(temp)
	print(histogram_final,np.array(histogram_final).shape)
	pickle.dump(histogram_final,open(outputpath, 'wb') )
	#'./histogram/'+name+'_histogram_final_0_'+str(_MIDDLE)+'_train.txt'

if __name__ == '__main__':
     #python histogramApi.py ./video1.txt,./ 180 ./histogram.txt
    print(sys.argv)
    genhist(sys.argv[1],sys.argv[2],sys.argv[3],sys.argv[4])







