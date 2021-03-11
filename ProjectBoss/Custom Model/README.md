# RandomRange ML Model

The script [```pb_generate_rndrange_model.py```](./pb_generate_rndrange_model.py) can be used to generate a machine learning ```.csv``` file that contains the data necessary for the game to take in and create a new final cost for the GOAP actions. 

It will populate an action's properties with a random value from within a given range, as well as create a final cost relevant to the action's properties. The final cost is influenced on certain factors, such as if the action deals damage or if the action was successful

The actions are the converted into a [pandas](https://pandas.pydata.org/) dataframe which allows it to be oriented the correct way for saving to a ```.csv``` for machine learning.

## Parameters

Near the top of the script are script-wide variables that can be used to customize the script, such as set minimum and maximum values for each action property, or the output ```.csv``` file.

The amount of data the script creates can be set on the ```ACTIONS_AMOUNT``` variable.