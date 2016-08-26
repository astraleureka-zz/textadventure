# Text Adventure

A simple text adventure engine in C, based on [Learn C The Hard Way, Exercise 19](http://c.learncodethehardway.org/book/ex19.html). 

## Dependencies
- libjansson 2.6 or newer
- cxref (for building documentation, optional, disable by passing **DOCS=0** to make)

## Getting Started

After checking out the source, just run 

```
make
```

to build the code. By default, documentation is enabled. To build without docs, pass **DOCS=0** to make:
```
make DOCS=0
```

## Test map layout

|    | 25 | 24 | 23 | 22 |    |    |    |
|    | 26 |    |    | 21 | 40 | 39 |    |
|    | 27 |    | 19 | 20 |    | 38 |    |
|    | 28 |    | 18 |    |    | 37 |    |
|    | 29 |    | 13 |    |    | 36 |    |
| 16 | 15 | 14 | 12 |    |    | 35 | 34 |
| 17 |    |    | 11 |    |    |    | 33 |
|    |    |    | 6  | 7  | 8  |    | 32 |
|    |    |    | 5  |    | 9  | 30 | 31 |
|    |    |    | 4  | 3  | 10 |    |    |
|    |    |    |    | 2  |    |    |    |
|    |    |    |    | 1  |    |    |    |

Once the assets are created, simply run ./game. To modify the map, monsters or items, edit the respective files in items/ mobs/ or rooms/. 
The parameters for each option are described in the file and have working defaults to demonstrate how it works.

## Todo

- More detailed character/mob mechanics (accuracy, proper critical hits, player defense)
- Player inventory
- Event triggers (e.g. unlock a door from another room)

## License

           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
                   Version 2, December 2004

Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>

Everyone is permitted to copy and distribute verbatim or modified
copies of this license document, and changing it is allowed as long
as the name is changed.

           DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
  TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION

 0. You just DO WHAT THE FUCK YOU WANT TO.

