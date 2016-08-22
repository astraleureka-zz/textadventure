# Text Adventure

A simple text adventure engine in C, based on [Learn C The Hard Way, Exercise 19](http://c.learncodethehardway.org/book/ex19.html). 

## Getting Started

After checking out the source, just run 

```
make
```

to build the code. By default, documentation is enabled. To build without docs, pass **DOCS** to make:
```
make DOCS=0
```

Before you can run the game you will need to build the assets:

```
CURRENTLY NO TOOLING, HAND EDIT FILES IN items/ mobs/ rooms/ 
```

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

