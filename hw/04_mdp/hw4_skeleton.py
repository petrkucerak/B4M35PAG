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
import queue
import sys
sys.setrecursionlimit(20000)
 
GO_UP = 0
GO_RIGHT = 1
GO_DOWN = 2
GO_LEFT = 3
WALL = 5
GOAL = 6
CONVERGENCE_DELTA = 0.005

class MDPState:
    """State class for a given space in gridworld, with directional attributes pointing to other squares.
    Each directional attribute is a tuple of coordinates (x, y). """

    def __init__(self, up, down, left, right, reward=-1, value=0):
        self.up = up
        self.down = down
        self.left = left
        self.right = right
        self.reward = reward
        self.value = value

    def __str__(self):
        return str(self.value)
 
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
    grid = [[' ' for _ in range(width)] for _ in range(height)]
    goals = queue.Queue()
    for r in range(height):
        for c in range(width):
            if maze[r][c] == 1:
                grid[r][c] = '#'
            elif maze[r][c] == 2:
                goals.put([r,c])
    return grid, goals

def maze_to_mdp(maze, goals):
    
    """Returns a matrix of MDPState objects for each free space in a maze"""
    
    grid = deepcopy(maze)

    for i in range(1, len(maze) - 1):
        for j in range(1, len(maze[i]) - 1):
            
            #represent walls as #
            if maze[i][j] == '#':
                grid[i][j] = '#'
                continue
                
            if maze[i-1][j] == '#':
                north = (i, j)
            else:
                north = (i-1, j)
                
            if maze[i+1][j] == '#':
                south = (i, j)
            else:
                south = (i+1, j)
                
            if maze[i][j+1] == '#':
                east = (i, j)
            else:
                east = (i, j+1)
                
            if maze[i][j-1] == '#':
                west = (i, j)
            else:
                west = (i, j-1)
                
            grid[i][j] = MDPState(north, south, west, east)
            
    while not goals.empty():
        i, j = goals.get()
        grid[i][j].reward = 10
            
    return(grid)

def policy_iteration(grid, gamma):
    """
    Performs policy iteration on a given grid of MDPState objects.
    """
    
    is_policy_changed = True
    
    policy = [['up' for i in range(len(grid[0]))] for j in range(len(grid))]
    actions = ['up', 'down', 'left', 'right']
    
    iterations = 0
    
    # Policy iteration
    while is_policy_changed:
        is_policy_changed = False
        # Policy evaluation
        # Transition probabilities not shown due to deterministic setting
        is_value_changed = True
        while is_value_changed:
            is_value_changed = False
            # Run value iteration for each state
            for i in range(len(grid)):
                for j in range(len(grid[i])):
                    if grid[i][j] == '#':
                        policy[i][j] = '#'
                    else:
                        neighbor = getattr(grid[i][j], policy[i][j])
                        v = grid[i][j].reward + gamma * grid[neighbor[0]][neighbor[1]].value
                        # Compare to previous iteration
                        if v != grid[i][j].value:
                            is_value_changed = True
                            grid[i][j].value = v
                                
        # Once values have converged for the policy, update policy with greedy actions
        for i in range(len(grid)):
            for j in range(len(grid[i])):
                if grid[i][j] != '#':
                    # Dictionary comprehension to get value associated with each action
                    action_values = {a: grid[getattr(grid[i][j], a)[0]][getattr(grid[i][j], a)[1]].value for a in actions}
                    best_action = max(action_values, key=action_values.get)
                    # Compare to previous policy
                    if best_action != policy[i][j]:
                        is_policy_changed = True
                        policy[i][j] = best_action
                        
        iterations += 1
                    
    return(policy)

def main(instance_path, solution_path):
    
    # Read instance and prepare policies and values
    maze, goals = init(instance_path)
    grid = maze_to_mdp(maze, goals)
    final_policy = policy_iteration(grid, .9)

    for r in range(len(grid)):
        for c in range(len(grid[r])):
            if final_policy[r][c] == '#': final_policy[r][c] = WALL
            elif grid[r][c].reward == 10: final_policy[r][c] = GOAL
            elif final_policy[r][c] == 'up': final_policy[r][c] = GO_UP
            elif final_policy[r][c] == 'left': final_policy[r][c] = GO_LEFT
            elif final_policy[r][c] == 'right': final_policy[r][c] = GO_RIGHT
            elif final_policy[r][c] == 'down': final_policy[r][c] = GO_DOWN


    # Todo : Implement policy itteration on CUDA using numba library

    # Save results
    with open(f'{solution_path}', 'w') as file:
        for row in final_policy:
            file.write(' '.join([str(item) for item in row]))
            file.write('\n')
     
if __name__ == "__main__":
    if len(sys.argv) > 1:
        instance_path = sys.argv[1]
        solution_path = sys.argv[2]
    else:
        print("Debug mode")
        instance_path = "instances/public_1.txt"
        solution_path = "out.txt"
    main(instance_path,solution_path)