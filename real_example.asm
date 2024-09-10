

global main
extern loop

section .text
    



section .data
    point_1x: .word 5   point_1y: .word 2
    point_2x: .word 8   point_2y: .word 3
    point_3x: .word 4   point_3y: .word 45
    point_4x: .word 0   point_4y: .word 500
    point_5x: .word 3   point_5y: .word 20
    point_6x: .word 300 point_6y: .word 15
    
*- 
   This is an implementation of an algorithm that solves
   the closest pair of points problem.
   https://en.wikipedia.org/wiki/Closest_pair_of_points_problem
-*

