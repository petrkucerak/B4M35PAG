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
    dmap = ['↑', '→', '↓', '←'] 
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
    policy = np.random.randint(4, size=(width, height)) # Random policy
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

    is_polic_changed = True

    # Todo : Implement policy itteration on CUDA using numba library

    width = values.shape[0]
    height = values.shape[1]


    while is_polic_changed:
        is_polic_changed = False
        plot_policy(policy)
        
        for r in range(height): # row
            for c in range(width): # col
                if policy[r][c] == WALL: continue
                straight = 0.8
                turn_left = turn_right = 0.1
                best_direction = 0
                if policy[r][c] == GO_DOWN:
                    straight *= values[r+1][c]
                    turn_left *= values[r][c+1]
                    turn_right *= values[r][c-1]
                    best_direction = max(turn_left, turn_right, straight)
                    if turn_left == best_direction: policy[r][c] = GO_RIGHT
                    elif turn_right == best_direction: policy[r][c] = GO_LEFT
                    
                elif policy[r][c] == GO_RIGHT:
                    straight *= values[r][c+1]
                    turn_left *= values[r-1][c]
                    turn_right *= values[r+1][c]
                    best_direction = max(turn_left, turn_right, straight)
                    if turn_left == best_direction: policy[r][c] = GO_UP
                    elif turn_right == best_direction: policy[r][c] = GO_DOWN

                elif policy[r][c] == GO_UP:
                    straight *= values[r-1][c]
                    turn_left *= values[r][c-1]
                    turn_right *= values[r][c+1]
                    best_direction = max(turn_left, turn_right, straight)
                    if turn_left == best_direction: policy[r][c] = GO_LEFT
                    elif turn_right == best_direction: policy[r][c] = GO_RIGHT

                elif policy[r][c] == GO_LEFT:
                    straight *= values[r][c-1]
                    turn_left *= values[r+1][c]
                    turn_right *= values[r-1][c]
                    best_direction = max(turn_left, turn_right, straight)
                    if turn_left == best_direction: policy[r][c] = GO_DOWN
                    elif turn_right == best_direction: policy[r][c] = GO_UP

                values_2[r][c] = values[r][c] + (CONVERGENCE_DELTA * best_direction)
                if values_2[r][c] != values[r][c]: is_polic_changed = True
        # copy values_2 to values
        values = values_2
                

                    

    final_policy = policy
    plot_policy(policy)
    
    # Save results
    with open(f'{solution_path}', 'w') as file:
        for row in final_policy:
            file.write(' '.join([str(item) for item in row]))
            file.write('\n')
     
if __name__ == "__main__":
    instance_path = sys.argv[1]
    solution_path = sys.argv[2]
    main(instance_path,solution_path)