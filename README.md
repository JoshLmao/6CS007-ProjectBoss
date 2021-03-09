# Project Boss (6CS007)

6CS007 year-long project artefact for implementing goal-oriented action planning (GOAP) inside UE4, where the agent is able to adapt to the enemy player's skill level. Created using Unreal Engine 4.25.4

## Plugins

This project was made using the follow plugins:

* [GOAP NPC Plugin for Unreal Engine](https://github.com/Narratech/TFGRomeroSanchezSierra/)
	* (This project contains edits to this plugin. Check [```/ProjectBoss/Plugins/GOAP/```](./ProjectBoss/Plugins/GOAP/) for source)
* [UnrealEnginePython](https://github.com/20tab/UnrealEnginePython)
	* (Using [davidjo](https://github.com/davidjo)'s '[Building4.25](https://github.com/20tab/UnrealEnginePython/pull/850)' branch)

## Project Install Instructions

### Asset Prerequisites
Make sure you install the ["Prerequisite Content"](./Prerequisites.md) for the project that includes certain models, animations and content that are too big for the repository.

### Machine Learning & Python

To use machine learning, python is used with [UnrealEnginePython](https://github.com/20tab/UnrealEnginePython). 

#### Python Directory
You can specify which Python is used by changing the ```pythonHome``` variable inside ```/ProjectBoss/Plugins/UnrealEnginePython/Source/UnrealEnginePython/UnrealEnginePython.Build.cs```

#### Requirements
Install the requirements needed by running the [install-py-requirements.bat](./ProjectBoss/install-py-requirements.bat) file. To view what requirements are needed, click to view [py-requirements.txt](./ProjectBoss/py-requirements.txt)

## Packaging

Check out the [package guide](./Packaging.md) for more information on deploy/packaging/shipping the game

## Questionnaire

Once users have played at least 5 games of Project Boss, they are asked to complete [this questionnaire form](https://form.questionscout.com/604213e1c441eb9aa5882081) for feedback. The questions are relevant to their experience and also gives the users a chance to provide feedback on the game.