'''
Project Boss: Machine Learning Model Creator
Josh Shepherd
https://github.com/JoshLmao/6CS007-ProjectBoss

Script to create an X amount of GOAP actions that contain the properties required for machine learning.
An action has: 
- BaseCost
- WasActionSuccessful
- Damage
- ExecutionSeconds
- FinalCost
The action properties are random numbers within the given min/max ranges. 
Certain actions will only deal damage. If the action doesn't deal damage, then damage will be 0.
All actions then get wrote to a CSV file, ready to be loaded by ML
'''
# Import necessary libraries
import random
import decimal
import pandas as pd
import os 

'''
General parameters
'''
# Amount of actions to generate and write to CSV file
ACTIONS_AMOUNT = 10
# Should an action be printed once it's generated?
DEBUG_PRINT_ACTION_PROPERTIES = False
# Single dimension array of strings that are names of all the GOAP actions available in Project Boss
ALL_ACTION_NAMES = [ "advanced-attack", "critical-melee", "follow", "heal", "invis-relocate", "melee-attack", "seek-cover", "ultimate" ]
'''
CSV file params
'''
# Directory and Name of CSV file to create
FILE_DIRECTORY = os.path.dirname(os.path.realpath(__file__)) + "/"
FILE_NAME = "pb-rndrange-model.csv"
# Full path of file
FULL_FILE_PATH = FILE_DIRECTORY + FILE_NAME
'''
Action Min/Max values params
'''
# BASE COST
BASE_COST_MIN = 5
BASE_COST_MAX = 40
# DAMAGE
DMG_MIN = 50
DMG_MAX = 400
# EXECUTE TIME (in seconds)
EXECUTE_MIN = 0
EXECUTE_MAX = 20
# FINAL COST
FINAL_COST_MIN = 1
FINAL_COST_MAX = 20
'''
Final Cost Params
'''
# Amount of time in seconds to favour the finalCost to apply the bonus
EXECUTE_TIME_FAVOURAILITY = 6.25
# Bonus multipluer to give if execute time is below EXECUTE_TIME_FAVOURAILITY time 
EXECUTE_TIME_BONUS = 1.35
# Multiplier bonus to cost if wasSuccess is true
SUCCESS_BONUS = 1.15
# Multiplier penalty to an action if it isn't successful
UNSUCCESSFUL_PENALTY = 0.45
# Multiplier penalty to an action with no player damage
NO_DMG_PENALTY = 0.65


'''
Returns a whole number (int) from rand range
'''
def rndRangeWholeNum(min, max):
    val = random.randint(min, max)
    return int(val)

'''
Determine an appropriate final cost from other action factors
'''
def determine_final_cost(baseCost, wasSuccess, dmg, execute_time):
    # Initially randomly assign final cost
    finalCost = 0
    # Get a random cost within range
    rnd_cost = random.randrange(FINAL_COST_MIN, FINAL_COST_MAX)
    # Multiply by a bonus or penalty depending if action was a success
    if wasSuccess:
        rnd_cost = rnd_cost * SUCCESS_BONUS
    else:
        rnd_cost = rnd_cost * UNSUCCESSFUL_PENALTY
    # If execute time is less than threshold, give bonus
    if execute_time <= EXECUTE_TIME_FAVOURAILITY:
        rnd_cost = rnd_cost * EXECUTE_TIME_BONUS
    # If action has no dmg (doesnt impact player), give penalty
    if dmg <= 0:
        rnd_cost = rnd_cost * NO_DMG_PENALTY
    # Set final cost to tuned cost
    finalCost = rnd_cost
    # Return final cost rounded to 7 decimal places max
    return float("{:.7f}".format(finalCost))

'''
Generates a list of actions with random values within the constraints. Creates the length amount of actions and returns all actions
'''
def gen_rnd_actions(length, allActionNames):
    # Store master list
    allActions = []

    # generate length amount of actions with random values
    for iter in range(length):
        # Determine action
        nameIndex = random.randrange(0, len(allActionNames))
        actName = allActionNames[nameIndex]
        # Check if ability is one that deals dmg
        bAbilDealsDmg = actName == "advanced-attack" or actName == "critical-melee" or actName == "melee-attack" or actName == "ultimate"
        # Determine base cost
        base_cost = rndRangeWholeNum(BASE_COST_MIN, BASE_COST_MAX)
        successful = rndRangeWholeNum(0, 1)

        # Determine dmg for action. 0 if action doesnt deal dmg
        damage = 0
        if bAbilDealsDmg:
            damage = rndRangeWholeNum(DMG_MIN, DMG_MAX)

        # Determine time to execute
        execute_time = decimal.Decimal(random.randrange(EXECUTE_MIN * 100, EXECUTE_MAX * 100)) / 100
        # Determine the final cost of the action
        final_cost = determine_final_cost(base_cost, successful, damage, execute_time)

        # Map properties to action
        action = {
            'baseCost': base_cost,
            'didSucceed': successful,
            'damage': damage,
            'averageExecuteSeconds': execute_time,
            'finalCost': final_cost
        }
        # Append to master list
        allActions.append(action)
        # Print to screen for debug
        if DEBUG_PRINT_ACTION_PROPERTIES:
            print("Generated '", actName, "' with base_cost '", base_cost, "' successful: '", successful, "' dmg: '", damage, "' executeTime: '", execute_time, "' | final_cost: '", final_cost, "'")
    # Return all actions after loop
    return allActions

'''
Saves the list to the gievn csv file path
'''
def save_to_csv(csvFullFilePath, list):
    # Convert list to dataframe
    df = pd.DataFrame(list)
    # Transpose so data goes horizontally instead of vertically
    df = df.T
    # Save to csv
    df.to_csv(csvFullFilePath, index=False, header=None, mode='a')

'''
Main
'''
def main():
    # Get random actions for length
    rndActions = gen_rnd_actions(ACTIONS_AMOUNT, ALL_ACTION_NAMES)
    print("Genereated '", len(rndActions), "' actions")
    print("Saving to '", FULL_FILE_PATH, "'")

    # Write ML header
    with open(FULL_FILE_PATH, 'w') as f:
        f.write('baseCost,didSucceed,damage,averageExecuteSeconds,finalCost\n')

    # Write data to file
    save_to_csv(FULL_FILE_PATH, rndActions)
    # Print output file location
    print("Saved CSV to '", FULL_FILE_PATH, "'")

# Run main
main()