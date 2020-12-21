import os
import pickle
import numpy as np


def cos_sim(a, b):
    a_norm = np.linalg.norm(a)
    b_norm = np.linalg.norm(b)
    cos = np.dot(a,b)/(a_norm * b_norm)
    return cos

def get_sample_label(v_r,vs):
    sims = []
    labels = []
    v_r = np.array(v_r[0])
    for (vec,label) in vs:
        vec = np.array(vec[0])
        sim = cos_sim(v_r,vec)
        sims.append(sim)
        labels.append(label)
    sim_args = np.argsort(-np.array(sims));
    return labels[sim_args[0]], sims[sim_args[0]]

if __name__ == '__main__':
    vectorDir = '../../dataset/3dSkeletonGait/v2/vector'
    files = os.listdir(vectorDir)
    vectors = []

    for file in files:
        if not 'p' in file:
            continue
        filePath = os.path.join(vectorDir,file)
        vector = pickle.load(open(filePath,'rb'),encoding='iso-8859-1')
        vectors.append((vector,file))

    results = []
    for (vector,file) in vectors:
        vector_archor = np.array(vector[0])
        negative_max = 0.0
        positive_min = 1.0
        sims=[]
        sims_list=[]
        files_list=[]
        for (vector_c,file_c) in vectors:
            vector_compare = np.array(vector_c[0])
            sim = cos_sim(vector_archor,vector_compare)
            sims.append((file_c,sim))
            sims_list.append(sim)
            files_list.append(file_c)

            if file_c.split('v')[0].split('p')[1]==file.split('v')[0].split('p')[1] and sim<positive_min:
                positive_min = sim
            elif file_c.split('v')[0].split('p')[1]!=file.split('v')[0].split('p')[1] and sim>negative_max:
                negative_max = sim
        result=''
        sim_args = np.argsort(-np.array(sims_list));
    #total result
    #for (file_c,sim) in sims:
    #    result+='['+file_c+'|'+'{:.2f}'.format(sim)+']'
    #ranked result top - 5
        for i in range(0,5):
            result += '[' + files_list[sim_args[i]] + '|' + '{:.2f}'.format(sims_list[sim_args[i]]) + ']'
        result = file+' Similar Score:'+result+'[NegMax:'+'{:.2f}'.format(negative_max)+'][PosMin:'+'{:.2f}'.format(positive_min)+']\n'
        print(result)
        results.append(result)
    outFile = open(os.path.join(vectorDir,'01result.txt'),'w')
    outFile.writelines(results)
    outFile.close()
