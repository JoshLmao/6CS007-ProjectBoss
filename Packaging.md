# Packaging Project Boss

## Macro Flags

Inside [ProjectBoss.Build.cs](./ProjectBoss/Source/ProjectBoss/ProjectBoss.Build.cs), there are a couple macro flags that can be enabled/disabled to change behaviour

## After Packaging

Make sure the build directory includes the ```project-boss-ml-data.csv``` file, otherwise no ML data will be used to train and tune the boss' performance. Also, make sure to copy the ```install-py-requirements.bat``` and ```py-requirements.txt``` files to the root of the build .exe. 

The ```install-py-requirements.bat``` file needs to be run before the .exe to install the required packages for Python.