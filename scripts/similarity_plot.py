# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np

def main():
    mhsn = read_similarity_analysis_file('output/sa_co1121.txt')
    pattern, names = read_pattern_analysis('output/pa_co1121.txt')
    fig, ax = plt.subplots(1, 1, dpi=144, figsize=(12,12))
    plot_matrix(ax, mhsn, names)
    
    mhsn = read_similarity_analysis_file('output/sa_rh111.txt')
    pattern, names = read_pattern_analysis('output/pa_rh111.txt')
    fig, ax = plt.subplots(1, 1, dpi=144, figsize=(5,5))
    plot_matrix(ax, mhsn, names)

def read_similarity_analysis_file(filename):
    f = open(filename)
    
    # skip header
    for i in range(0,8):
        f.readline()
        
    # read number of atoms
    nratoms = int(f.readline().split()[-1])
    
    # skip line
    f.readline()
    
    # create container
    mhsn = np.zeros((nratoms, nratoms))
        
    # read atomic data
    for i in range(0, nratoms):
        for j in range(0, nratoms):
            pieces = f.readline().split()
            atid1 = int(pieces[0]) - 1
            atid2 = int(pieces[1]) - 1
            
            if(pieces[-1] == 'N/A'):
                m = 0
            else:
                m = float(pieces[-3])
            mhsn[atid1,atid2] = m
            
    return mhsn
 
def read_pattern_analysis(filename):
    f = open(filename)
    
    # skip header
    for i in range(0,8):
        f.readline()
        
    # read number of atoms
    nratoms = int(f.readline().split()[-1])
    
    # skip lines
    for i in range(0,3):
        f.readline()
    
    # create containers
    patterns = []
    names = []
    
    for i in range(0, nratoms):
        pieces = f.readline().split()
        pattern = pieces[-1]
        name = " ".join(pieces[6:-1])
        
        patterns.append(pattern)
        names.append(name)
        
    return pattern, names
           
def plot_matrix(ax, mat, labels, title = None, xlabelrot = 90):
    """
    Produce plot of matrix
    """
    ax.imshow(mat, vmin=0, cmap='RdPu')
    for i in range(mat.shape[0]):
        for j in range(mat.shape[1]):
            ax.text(i, j, '%.1f' % mat[j,i], ha='center', va='center',
                    fontsize=5, color='black' if mat[j,i] < 10 else 'white')
    ax.set_xticks([])
    ax.set_yticks([])
    ax.hlines(np.arange(1, mat.shape[0])-0.5, -0.5, mat.shape[0] - 0.5,
              color='black', linestyle='--', linewidth=1)
    ax.vlines(np.arange(1, mat.shape[0])-0.5, -0.5, mat.shape[0] - 0.5,
              color='black', linestyle='--', linewidth=1)
    
    # add basis functions as axes labels
    ax.set_xticks(np.arange(0, mat.shape[0]))
    ax.set_xticklabels(labels, rotation=xlabelrot)
    ax.set_yticks(np.arange(0, mat.shape[0]))
    ax.set_yticklabels(labels, rotation=0)
    ax.tick_params(axis='both', which='major', labelsize=7)
    
    # # add title if supplied
    # if title:
    #     ax.set_title(title)


if __name__ == '__main__':
    main()