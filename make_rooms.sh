#!/bin/sh
rm -rf rooms/
mkdir rooms/
touch rooms/.placeholder
#                         room_id  mob_id   north   south    east    west	name					description
./roomtool rooms/origin		1	0	2	0	0	0	"the spawn room" 			"There doesn't seem to be anything here."
./roomtool rooms/first		2	1	4	1	0	3	"the first room" 			"This room is as drab and unoriginal as where you started."
./roomtool rooms/first_l	3	3	27	0	2	0	"an armory" 				"Dozens of swords, glaives and axes adorn the walls. The glimmer of all this pristine weaponry gives you excitement. Too bad you've already got the broadsword."
./roomtool rooms/recov_1	27	0	0	3	0	28	"the alchemist's office"		"The alchemist's office is filled with carafes, jars and flasks, all swimming with wildly varying colours and odors."
./roomtool rooms/trt		28	4	0	0	27	0	"a hallway"				"Just an ordinary dead end, leading to a washroom. Nothing of interest here."
./roomtool rooms/second		4	3	5	2	0	0	"an excessively long hallway" 		"This hall is insane. You stare off into the distance, and you swear you can see fog."
./roomtool rooms/subboss	5	8	0	4	6	0	"the demon's lair" 			"The stench of the demon permeates the room, reminiscent of putrid flesh. What appears to be dried blood encrusts all surfaces."
./roomtool rooms/subboss_1_c	6	28	0	0	0	5	"the end of the line"			"sorry, this is all the patience I had to write script"
