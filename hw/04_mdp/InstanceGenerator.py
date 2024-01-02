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
 
 
# From HW assignment
 
class MDPState:
    def __init__(self, up, down, left, right, reward=-1, value=0):
        self.up = up
        self.down = down
        self.left = left
        self.right = right
        self.reward = reward
        self.value = value
 
    def __str__(self):
        return str(self.value)
 
 
class Maze:
    """
    Adapted from http://rosettacode.org/wiki/Maze_generation#Python
    """
 
    def __init__(self, w=10, h=10, children=None):
        self.vis = [[0] * w + [1] for _ in range(h)] + [[1] * (w + 1)]
        self.ver = [["# "] * w + ['#'] for _ in range(h)] + [[]]
        self.hor = [["##"] * w + ['#'] for _ in range(h + 1)]
        self.matrix = self.make_maze(w, h)
        self.grid = self.maze_to_mdp()
 
    def make_maze(self, w, h):
        self.walk(randrange(w), randrange(h))
 
        cells = ""
 
        for (a, b) in zip(self.hor, self.ver):
            cells += ''.join(a + ['\n'] + b + ['\n'])
 
        cells = cells.split('\n')
        cells = [list(row) for row in cells]
        del cells[len(cells) - 1]
        del cells[len(cells) - 1]
 
        return [row for row in cells]
 
    def walk(self, x, y):
        self.vis[y][x] = 1
 
        d = [(x - 1, y), (x, y + 1), (x + 1, y), (x, y - 1)]
        shuffle(d)
        for (xx, yy) in d:
            if self.vis[yy][xx]: continue
            if xx == x: self.hor[max(y, yy)][x] = "# "
            if yy == y: self.ver[y][max(x, xx)] = "  "
            self.walk(xx, yy)
 
    def __repr__(self):
        strlst = [''.join(row) for row in self.matrix]
        return '\n'.join(strlst)
 
    def maze_to_mdp(self):
        grid = deepcopy(self.matrix)
 
        for i in range(1, len(self.matrix) - 1):
            for j in range(1, len(self.matrix) - 1):
 
                # represent walls as #
                if self.matrix[i][j] == '#':
                    grid[i][j] = '#'
                    continue
 
                if self.matrix[i - 1][j] == '#':
                    north = (i, j)
                else:
                    north = (i - 1, j)
 
                if self.matrix[i + 1][j] == '#':
                    south = (i, j)
                else:
                    south = (i + 1, j)
 
                if self.matrix[i][j + 1] == '#':
                    east = (i, j)
                else:
                    east = (i, j + 1)
 
                if self.matrix[i][j - 1] == '#':
                    west = (i, j)
                else:
                    west = (i, j - 1)
 
                grid[i][j] = MDPState(north, south, west, east)
 
        grid[1][1].reward = 10
 
        return (grid)
 
 
def generate_maze(n, filename):
    h = int(n / 2)
    w = int(n / 2)
    new_maze = Maze(w, h)
    np_maze = np.array(new_maze.matrix)
    my_maze = np.ones(np_maze.shape)
    my_maze[np_maze == ' '] = 0
    my_maze = my_maze.astype(np.int32)
 
    my_maze[1, 1] = 2
    min_val, max_val = 0, 1
    goal_states = max(1, randint(1, int(np.real(sqrt(max(int(my_maze.shape[0]), int(my_maze.shape[1])))))))
    for k in range(goal_states):
        new_goal = False
        while not new_goal:
            i = randint(0, my_maze.shape[0] - 1)
            j = randint(0, my_maze.shape[1] - 1)
            if my_maze[i, j] == 0:
                my_maze[i, j] = 2
                new_goal = True
 
    with open(filename, 'w') as f:
        f.write(str(int(my_maze.shape[0])) + " " + str(int(my_maze.shape[1])) + "\n")
        for i in range(my_maze.shape[0]):
            for j in range(my_maze.shape[1]):
                f.write(str(int(my_maze[i, j])) + " ")
            f.write("\n")

if __name__ == "__main__":
    generate_maze(int(sys.argv[1]),sys.argv[2])