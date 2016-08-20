#!/bin/sh
rm -rf mobs/
mkdir mobs/
touch mobs/.placeholder
#		         monster_id  gender   health  skill     str     def     cel     int item_id	name			name2				<attack str>							<defend str>									<description>
./mobtool mobs/ghoul		1	1	15	0	10	8	3	0	0	"a ghoul"		"the ghoul"			"lunges forth, clawing at your flesh"				"looks at you eye to eye, ready to absorb the impact of your blade"		"Your typical garden variety ghoul. Poor temper and outright disgusting to boot."
./mobtool mobs/golem		3	0	27	4	3	12	1	2	0	"a golem"		"the earthen golem"		"pelts you with chunks of rock"					"remains unmoving, with a stony gaze"						"An earthen golem, enchanted by a mysterious wizard. It looks at you with a lifeless stare."
./mobtool mobs/tanuki		4	2	7	0	4	2	6	1	0	"a tanuki"		"the raccoon dog"		"nips at your feet"						"jumps away from your strike"							"A bit like a fat fox crossed with a raccoon. Mostly harmless."
./mobtool mobs/pixie		8	1	16	12	27	1	10	7	0	"a pixie"		"the pixie"			"jolts you with faerie magicks"					"zips around your head faster than you can keep track of it"			"The most devilish little creature, born of mayhem and torment. Pixies are well known for all kinds of mischief from playing harmless pranks to kidnapping small children."
./mobtool mobs/dragon		28	2	7	30	21	6	4	5	0	"the dragon"		"the unholy dragon"		"blasts you with a mouthful of firey gas"			"hovers just out of your reach"							"Who on earth let a dragon into this castle? And how? Ah... none of that's important right now, this thing is PISSED."
