# Goal Oriented Action Planning (GOAP)

The boss' actions are split up for use with GOAP.

## World State & Targets

The GOAP world has 5 states that it uses to go through the setup actions. These states can be seen in the [```AGOAPAIController::GetDefaultWorldState()```](./GOAPAIController.cpp#L317) function. Below is a topdown view of all of the actions and their preconditions and effects.

![GOAP actions precondition and effects]() 

The world mainly gets set to always use the ```damage-player``` state to perform the actions. However, as the boss can heal, if the Boss' heal ability requirements are satisfied, then the ```heal``` state will be set to allow the Boss to perform the Heal action. The world state is determined within the [```AGOAPAIController::DetermineNextWorldState()```](./GOAPAIController.cpp#L276) function

## Project Boss Actions

The boss' actions override and add additional functionality to the base GOAP system's ```UGOAPAction```, located inside [```UPBGOAPAction```](./PBGOAPAction.h). 

Each ```UPBGOAPAction``` has a ```BaseCost```, which is a arbitrary value that can be set. The action can also have it's cost adjusted using the ```UpdateCost(float additionalCost)``` function which allows the action to be more likely to be executed.

Each ability action in their ```checkProceduralPrecondition(APawn* p)``` function mostly checks for the same criteria. 

* Actions attempts to get and set the ```targetsType```
    * Uses the ```TrySetTarget(APawn* pawn)``` function in ```UPBGOAPAction```
* Action checks if the character is performing any other action
    * Mostly a safety check as to not override an action in progress
* Action checks if the action is on cooldown 
    * Cooldown's stored in character
* Action checks for any other required conditions to allow execution
    * Character is more than ```X``` units away from target

If all criteria is successful, the action will be allowed to execute