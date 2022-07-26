# GomokuAI-in-Cpp
This is a mini project from NTHU Introduction to Programming2 course. In this project, I design and implement an A.I. that can play the boardgame Gomoku.
In this project, the game runner (main.cpp) executes the AIs of the player and the opponent in turns and communicates with them by files. 
The game AIs should read the board status from the file “state”, and output the move it made to the file “action”.
I implement the Alpha-Beta pruning algorithm so that my AI is able to look forward for more steps and simulate how the opponent thinks to make the best choice with least risk.
And I also inplement a value function to evaluate the score of the board to help my AI to design which move to take.



