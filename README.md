# Project Boss (6CS007)


[![releases](https://img.shields.io/badge/download-/releases/-green.svg?style=flat-square)](https://github.com/JoshLmao/6CS007-ProjectBoss/releases)
[![contact-twitter](https://img.shields.io/badge/twitter-JoshLmao-blue.svg?style=flat-square&logo=twitter)](https://twitter.com/joshlmao)
[![contact-website](https://img.shields.io/badge/website-joshlmao.com-lightgrey.svg?style=flat-square)](http://joshlmao.com)

6CS007 year-long project artefact for implementing goal-oriented action planning (GOAP) inside UE4, where the agent is able to adapt to the enemy player's skill level by using machine learning to adjust costs and perform actions. Created using Unreal Engine 4.25.4

## Project Aim

The aim of my project and this artefact is to see if I can use goal oriented action planning (GOAP) and machine learning to create an enemy that is able to adapt itself to the player, adjusting it's play style to match and create a challenging, but defeatable enemy. 

<p align="center">
	<a href="https://youtu.be/kER2ndhfFOw">
		<img src="https://user-images.githubusercontent.com/22716832/111204124-b95b1780-85bd-11eb-956b-1dcb62519e58.jpg" width="80%">
	</a>
</p>

## GOAP/ML Implementations

For more information on the implementations of GOAP and ML, please read the specific ```README.md``` files for each.

* [GOAP](./ProjectBoss/Source/ProjectBoss/Boss/GOAP/README.md)
* [Machine Learning](./ProjectBoss/Source/ProjectBoss/ML/README.md)

## Plugins

This project was made using the follow plugins:

* [GOAP NPC Plugin for Unreal Engine](https://github.com/Narratech/TFGRomeroSanchezSierra/)
	* (This project contains edits to this plugin. Check [```/ProjectBoss/Plugins/GOAP/```](./ProjectBoss/Plugins/GOAP/) for source)
* [UnrealEnginePython](https://github.com/20tab/UnrealEnginePython)
	* (Using [davidjo](https://github.com/davidjo)'s '[Building4.25](https://github.com/20tab/UnrealEnginePython/pull/850)' branch)
	* ([davidjo's Building4.25](https://github.com/JoshLmao/UnrealEnginePython-PB-4.25) branch cloned for backup)

## Project Install Instructions

### Setup Instructions

1. Clone the repository to your system
2. Install Unreal Engine 4.25.4 & Python [v3.6.8](https://www.python.org/downloads/release/python-368/)
3. Install the required prerequisites assets in [```Prerequisites.md```](./Prerequisites.md)
4. Run the [install-py-requirements.bat](./ProjectBoss/install-py-requirements.bat) file to install the required Python packages
	* Or install the Python requirements in [```py-requirements.txt```](./ProjectBoss/py-requirements.txt) if you can't run ```.bat``` files
5. Right click ```ProjectBoss.uproject``` and select ```Generate Visual Studio project files...```
6. Launch the project

#### Set your Python Directory for UnrealEnginePython
You can specify which Python is used by changing the ```pythonHome``` variable inside ```/ProjectBoss/Plugins/UnrealEnginePython/Source/UnrealEnginePython/UnrealEnginePython.Build.cs```

## Packaging

Check out the [package guide](./Packaging.md) for more information on deploy/packaging/shipping the game

## Logging

The project logs out useful gameplay information related to abilities, health, damage, machine learning, the GOAP system, etc. There are a handful of custom log categories that can be used to filter the logs. The full list can be [found here](./ProjectBoss/Source/ProjectBoss/ProjectBoss.cpp) under the region, ```Logs```.

In the packaged build, the logs can be found at ```[PackagedBuild]/ProjectBoss/Saved/Logs/```

## Questionnaire

Once users have played at least 5 games of Project Boss, they are asked to complete [this questionnaire form](https://form.questionscout.com/604213e1c441eb9aa5882081) for feedback. The questions are relevant to their experience and also gives the users a chance to provide feedback on the game.
