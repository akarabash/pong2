# MSP430-Ping-Pong-Game
Ping Pong Game I made for the MSP430 Board for my Computer Architecture class

# Project 3: Ping Pong Game
## Introduction
- I created game of ping pong on the msp430, I used Professors Fraudenthals code from the shapemotion.c demo
and changed the code to crete the game, Fraudenthals code had the frame for the ping pong game, I added the
layout for the red and blue paddle, using the rectangle layout in the demo code, and changed the dimension 
to be the paddle. The ball layout was changed to be a smaller size ball, and to bounce around in the frame, 
in my game the balls bounce on the left and right walls, and it is considered a point if the ball touches
the top or botton walls. The paddes are designed to make the ball bounce if the ball touches the paddle.

- To run the program you need to make all from the original file in the repisotory

$ make all

- and goto shapemotion file and call 

$ make clean 

$ make load

- switches s1 and s2 move red paddle left and right and s3 and s4 move the blue paddle left and right.

- Any of the two players that score 4 points will win the game and a gameover followed by which player 
won will print in the screen, 

- I used as reference Professors Fraudenthals code from project 2 and project to create the gaem, 
I used the buzzer_set_cycles method to generate sound from the buzzer and used the shapemotion demo as a structure for my pong game. I talked pseuo code with my follow classmates Freddy Garcia, Francisco Landa, 
Aaron Santillanes, and some other students from Computer Architecture Class.
