# minesweeper
minesweeper for CLI on Linux

Now you can play minesweeper directly in the terminal!
For the starting point introduce the line, then the column which you want to mine first.
For the rest of the inputs the first character is the action type followed by the row and then by column.
Actions include:
 - Mine: clears the selected square
 - Chord: if there number of flags around the square is equal to the number of the square where this action takes place it will clear all unflaged squares around it (can only be used on a revealed square)
 - Flag: flags a square if you think there is a bomb there (or removes flag if already placed)
 - Unsure: puts a question mark on a square (or removes question mark if already placed)
