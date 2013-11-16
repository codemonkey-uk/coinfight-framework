coinfight-framework
===================

AI &amp; Tournament Framework for Coin Fight, a turn based game.
Coin Fight, aka Pennywise, is a free game by Cheapass Games:
http://www.cheapass.com/node/28

The coinfight framework uses the original "Cody" starting coin set:
4x1&cent;, 3x5&cent;, 2x10&cent;, 1x25&cent;

## Adding New Bots in C/C++

See the examples in example.cpp (C++) or cplayers.c (C). 
Bots added this way become "build in" when the framework is compiled and are included by default when running a tournament.

## Command Line Bots

### Implementing Command Line Bots

The framework will run an external / command line bot for each turn. 
The current game state will be passed to the bot over "stdin" and the bots move will be read from "stdout".

#### Input Format

The first line contains two decimal values, P T, seperated by a space:

    \d+ \d+\n
  
The first, P, is the number of players in the game. The second, T, is the current turn number.
The current player can be determined by taking the remander of P / T, or P%T. Bots are only run on their own turn.

P+1 lines follow, each one containing a change pool. The first is the "table", into which players "pay", 
and from which players may take change. The remaining P lines are each of the other players current pool of coins.

A change pool is written as a single line of N comma seperated CxD pairs:

    (\d+x\d+, ){0,}\d+x\d+
    
Where C is Count and D is Denomination. In practice, N will be 4 and D will always be 1, 5, 10, 25, 
but handling this for other values of N and other values of D generally is recomended.

Putting it all together, the following regex should match a coinfight game state:

    \d+ \d+\n((\d+x\d+, ){0,}\d+x\d+\n){2,}
  
#### Output Format

The output should consist of two lines. The first containing the denomination of the coin the bot wants to play.
The second line being the change that the bot wishes to take. The bot must have 1+ coins of the denomination they wish
to play, and the change they take must (a) be less in value than the coin played, and (b) must be present in the 
game's "table" change pool.

    \d+\n(\d+x\d+, ){0,}\d+x\d+

#### Sample Input

    3 75
    8x1, 9x5, 6x10, 3x25
    1x1, 0x5, 0x10, 0x25
    0x1, 0x5, 0x10, 0x25
    3x1, 0x5, 0x10, 0x25

#### Sample Output

    1
    0x1, 0x5, 0x10, 0x25

#### Explanation

It is a 3 player game. It is the 75th turn. 75%3 = 0, so it is player 0s turn. The current change on the table is: 
8x1&cent;, 9x5&cent;, 6x10&cent;, &amp; 3x25&cent;. Player 0 has only got 1&cent; left, 
so they play the only move they can, 1&cent; and take no change. 
