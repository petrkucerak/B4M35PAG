#!/usr/bin/env python3
import math
import time
import numpy as np
import matplotlib.pyplot as plt
from numba import cuda
from matplotlib.colors import ListedColormap
from random import shuffle, randrange, randint
from copy import deepcopy
from cmath import sqrt
import sys
sys.setrecursionlimit(20000)
 
GO_UP = 0
GO_RIGHT = 1
GO_DOWN = 2
GO_LEFT = 3
WALL = 5
GOAL = 6
CONVERGENCE_DELTA = 0.01
 
def plot_policy(policy):
    '''
    Function for plot mace with given policy
    :param Policy: Policy for given maze
    '''
    cmap = ListedColormap(['w', 'w', 'w', 'w', 'w', 'k', 'g'])
    dmap = ['^', '>', 'v', '<'] 
    fig, ax = plt.subplots()
    ax.matshow(policy, cmap=cmap)   
    for (i, j), z in np.ndenumerate(policy):
        if z <= GO_LEFT:
            ax.text(j, i, dmap[z], ha='center', va='center')    
    plt.show()  

def init(filename):
    '''
    Function read instance from file and creates array of policies and values
    :param filename: Path to instance file
    '''
    f = open(filename, 'r')
    lines = f.readlines()
    f.close()   
    width, height = map(lambda x: int(x), lines[0].strip().split(' '))
    maze = list(map(lambda line: list(map(lambda x: int(x), line.strip().split(' '))), lines[1:height+1]))  
    policy = np.random.randint(4, size=(width, height))
    values = np.ones([width, height])
    values_2 = np.ones([width, height]) 
    for r in range(height):
        for c in range(width):
            if maze[r][c] == 1:
                policy[r][c] = WALL
                values[r][c] =   0
                values_2[r][c] = 0
            elif maze[r][c] == 2:
                policy[r][c] = GOAL
                values[r][c] =   1000000
                values_2[r][c] = 1000000 
    return maze, policy, values, values_2


def main(instance_path, solution_path):
    
    # Read instance and prepare policies and values
    maze, policy, values, values_2 = init(instance_path)
    
    final_policy = policy

    # Todo : Implement policy itteration on CUDA using numba library
    
    # Save results
    with open(f'{solution_path}', 'w') as file:
        for row in final_policy:
            file.write(' '.join([str(item) for item in row]))
            file.write('\n')
     
if __name__ == "__main__":
    instance_path = sys.argv[1]
    solution_path = sys.argv[2]
    main(instance_path,solution_path)