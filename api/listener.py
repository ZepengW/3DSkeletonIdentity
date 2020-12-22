import socket
from calculate_feature import get_label_score,save_label
import _thread
import os
import pickle
DATASETDIR = './dataset'

#socket 通信类
class SocketServer:
    def __init__(self):
        self.s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        host = '127.0.0.1'
        port = 1996     #端口号
        self.s.bind((host,port))
        self.s.listen()

    def __del__(self):
        self.s.close()

def convert_rawdata_to_skeleton(dataFromSocket):
    #data = dataFromSocket.decode('utf-8')
    data = dataFromSocket
    frameList = data.split(';')
    skeletonFrameList = []
    for frame in frameList:
        jointList = frame.split(',')
        if(18*3!=len(jointList)):
            continue
        jointList = [float(j) for j in jointList]
        skeletonFrameList.append(jointList)
    return skeletonFrameList

def detect_person(data,conn,vectors):
    skeletonFrameList = convert_rawdata_to_skeleton(data)
    # detect skeleton data
    label, score = get_label_score(skeletonFrameList, vectors)
    # send result
    responseMess = 'L'+str(label) + ':' + str(score)
    # responseMess = responseMess.decode('utf-8')
    conn.send(bytes(responseMess,encoding='utf-8'))
    print("send : "+responseMess)

def collect_data(data,conn,vectors):
    skeletonFrameList = convert_rawdata_to_skeleton(data.split('S')[1])
    label = data.split('S')[0]
    filename = save_label(skeletonFrameList,DATASETDIR,label)
    # send result
    responseMess = "save finish. save name:"+filename
    # responseMess = responseMess.decode('utf-8')
    conn.send(bytes(responseMess, encoding='utf-8'))
    print("send : " + responseMess)
    print("reload dataset")
    vectors = load_dataset(DATASETDIR)

def load_dataset(datasetDir):
    files = os.listdir(datasetDir)
    vectors = []
    for f in files:
        vector = pickle.load(open(os.path.join(datasetDir, f), 'rb'), encoding='iso-8859-1')
        vectors.append((vector, f.split('v')[0].split('p')[1]))
    return vectors

if __name__ == '__main__':

    server = SocketServer()
    print("load dataset")
    vectors = load_dataset(DATASETDIR)
    print("load dataset finish")
    while 1:
        print("listening ...")
        conn,addr = server.s.accept()
        print("client connected")
        while 1:
            data = ""
            data = conn.recv(100000)
            #process data to skeleton list
            if not data :
                break
            print("receive data :")
            print(data)
            data = str(data,encoding='utf-8')
            func_type = data[0]
            if(func_type=='Q'):
                _thread.start_new_thread(detect_person,(data[1:],conn,vectors))
            elif(func_type=='L'):
                collect_data(data[1:],conn,vectors)
                print("reload dataset")
                vectors = load_dataset(DATASETDIR)
                print("reload dataset finish")
        print("connect close")

