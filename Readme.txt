Controls for AI assignment 2
Spacebar to toggle on/off whether you are able to see the enemy

1: summon a random 10x10 maze
2: summon a random 12x12 maze

AI:
Player:- Player AI have 2 states, Finding and EscapeMaze
Finding: Uses DFS to search for the exit
EscapeMaze: Uses BFS to go towards the exit

Enemy:- Enemy AI also have 2 states, EnemyIdle and EnemyWonder
EnemyIdle: Idle for n number of turns before switching to EnemyWonder state
EnemyWonder: Wonder around the maze

GameLogic:
Due to some issues with the maze generator, game will end if player reaches (0,0) after the game started
Win:
Player wins the game when he reaches the exit safely
Lose: 
Enemy AI touches the player before the player reaches the exit