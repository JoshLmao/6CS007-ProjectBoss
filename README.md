# Project Boss (6CS007)

6CS007 year-long project artefact for implementing goal-oriented action planning (GOAP) inside UE4, where the agent is able to adapt to the enemy player's skill level. Created using Unreal Engine 4.25.4

## Project Install Instructions

### Asset Prerequisites
Make sure you install the ["Prerequisite Content"](./Prerequisites.md) for the project that includes certain models, animations and content that are too big for the repository.

### Machine Learning & Python

To use machine learning, python is used with UnrealEnginePython. 

#### Python Directory
You can specify which Python is used by changing the ```pythonHome``` variable inside ```/ProjectBoss/Plugins/UnrealEnginePython/Source/UnrealEnginePython/UnrealEnginePython.Build.cs```

#### Requirements
Install the requirements needed with the [install-py-requirements.bat](./ProjectBoss/install-py-requirements.bat) file.
