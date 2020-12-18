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

_SKNUM=18-1
_SKNUM1=13
_SKNUM2=18
_SKNUM3=18

_TEMPOLEN=8
#_FLEN=156
#_LEN=1696
_LEN=448*2
_CENTER1=256
_CENTER2=256

#32*4+32
#32*13=448
#1664+32=1696

#16*13*4=832
#832+16=848

#_MIDDLE=62
#_MIDDLE1=63

_MIDDLE=24


#_MIDDLE=74
#_MIDDLE1=75

_histsum=100

_root="/home/bird/xuke/gait/output_0510(1-40)/"
_rootout="/home/bird/xuke/gait/output_feature/"
_center_path='./center/'