Collated by Michael Oborn

Readme last edited 21/02/2024


This is the github repo for all technical documents used/required at Escape HQ

Currently this is just storing code, 
however in future should include circuit diagrams and hardware lists for each puzzle

Arduino sketches should be managed in VS code, through an extension called 'PlatformIO'
Use of PlatformIO and VS code allows for good integration with GitHub, for file/version management

PlatformIO also means that all libraries and dependencies for a sketch are stored in a larger project folder
I think ArduinoIDE does the same, but less directly compatible with GitHub. 

PlatformIO and VScode also makes coding easier if switching between C for arduino or python for raspberrypi
All coding can be done in the same IDE

To access any particular script, go {Room name} -> {Puzzle Name} -> src -> {Puzzle Name.cpp}

To flash to ardunio, you will need to have installed PlatformIO extension in VSCode. 
    Then open whichever PlatformIO project folder for the puzzle in question
        PlatformIO (from sidebar) -> open -> open project -> navigate to local folder

The reason why there is a seperate PlatformIO project created for each puzzle is incase puzzles use different Ardunio boards - they can be individually assigned
Also (just like arduino) it struggles when there's multiple 'main.cpp' files and cant flash to controller properly

.fzz files are fritzing files! these are circuit diagrams!!! I've mostly only found for Moo as of yet, but very good to have
we can continue this trend and create further circuit documents also in fritzing, rather than changing software
fritzing is a $15 fee to download
