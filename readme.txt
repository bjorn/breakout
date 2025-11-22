------------------------------------------------
A simple breakout game - using a particle engine
by Bjørn Lindeijer
------------------------------------------------

Needless to say, this game is not finished. It lacks many things like sounds, effects, graphics, main menu, end of game and powerups. But it also lacks decent collision handling (errors on colliding with two blocks at the same time) and it doesn't have Z-ordering. To fix the last two things my particle engine would require another major rewrite, and I might have to create a more treelike designed game instead of using linked lists all the time.

Nevertheless, the game is playable. The first level might be a bit boring (coins were supposed to scatter upon hitting a gold block), but it should be rewarding to play all three levels... and then pressing ESC to quit the game.


Compiling
---------
Just run "make". Windows EXE in included, but you will need all3935.dll to run it.


Controls
--------
release ball:     [space]
pad to the left:  [left arrow key]
pad to the right: [right arrow key]
quit game:        [esc]


Contact
-------
email:   bjorn@moonlightrpg.nl
website: http://www.moonlightrpg.nl/breakout
