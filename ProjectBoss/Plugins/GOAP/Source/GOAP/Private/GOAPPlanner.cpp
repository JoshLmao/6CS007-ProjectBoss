/**
	GOAP NPC: Goal-Oriented Action Planning for Non-Player Characters.

	Publishers/Authors:
		-	Diego Romero-Hombrebueno Santos.
		-	Mario Sanchez Blanco.
		-	Jose Manuel Sierra Ramos.

	Published on 2020.
 */
#include "GOAPPlanner.h"

GOAPPlanner::GOAPPlanner() {}

GOAPPlanner::~GOAPPlanner() {}

GOAPPlanner::GOAPPlanner(GOAPWorldState* c, GOAPWorldState* g, const TArray<UGOAPAction*>& a)
{
	currentWorld = c;
	goal = g;
	actions = a;
}

GOAPNode GOAPPlanner::lowestFinList(const TArray<GOAPNode>& opList) 
{
	GOAPNode node;

	float minF = MAX_FLT;
	for (GOAPNode n : opList) 
	{
		if ((n.getF()) < minF) 
		{
			node = n;
			minF = n.getF();
		}
	}

	return node;
}

bool containsNode(GOAPNode node, const TArray<GOAPNode>& list) 
{
	bool contains = false;
	for (GOAPNode n : list) 
	{
		if (n == node) 
		{
			contains = true;
			break;
		}
	}
	return contains;
}

TArray<GOAPNode> GOAPPlanner::getAdjacent(GOAPNode current, const TArray<UGOAPAction*>& vActions, APawn* p) 
{
	TArray<GOAPNode> adjacentNodes;
	GOAPNode adjacent;
	GOAPWorldState world;

	for (int i = 0; i < vActions.Num(); ++i) 
	{
		// Checks if the action can be performed from the current world.
		const bool bPredoncitionsAreMet = current.getWorld().isIncluded(vActions[i]->getPreconditions());
		
		// Checks if the action is the same as the current one. (This can be deleted if you want your AI to perform the same action consecutively).
		//const bool bSameActionAsBefore = current.getAction() == vActions[i];

		// Checks the procedural precondition of the action.
		const bool bProceduralPreconditionFulfilled = vActions[i]->checkProceduralPrecondition(p);
		
		if (bPredoncitionsAreMet /*&& !bSameActionAsBefore*/ && bProceduralPreconditionFulfilled)
		{
			world = current.getWorld(); // Saves the current world.
			world.joinWorldState(vActions[i]->getEffects()); // Applies effects of the action to the saved world.
			adjacent.setWorld(world); // Sets the adjacet node's world.
			adjacent.setAction(vActions[i]); // Sets the adjacet node's action. 
			adjacentNodes.Push(adjacent); // Includes the adjacent node in the list.
		}
	}
	return adjacentNodes;
}

TArray<UGOAPAction*> GOAPPlanner::generatePlan(APawn* p) 
{
	TArray<UGOAPAction*> sol;

	// Store start and last nodes. Set start as the current world
	GOAPNode startNode, lastNode;
	startNode.setWorld(*currentWorld); 
	startNode.setParent(-1);
	
	// Clear lists and start on startNode
	openList.Empty();
	closedList.Empty();
	openList.Push(startNode);

	bool continues = true;
	bool goalReached = false;

	// Search and create the cheapest path between actions having into account their preconditions, effects and cost.
	while (continues) 
	{
		GOAPNode currentNode = lowestFinList(openList);
		openList.Remove(currentNode);
		closedList.Push(currentNode);
		int pos = closedList.Num() - 1;
		
		// When the current plan reaches the goal, the plan stops.
		if (currentNode.getWorld().isIncluded(*goal)) 
		{
			lastNode = currentNode;
			continues = false;
			goalReached = true;
			break;
		}
		// Get adjacents of actual node.
		TArray<GOAPNode> adjacents = getAdjacent(currentNode, actions, p);

		// Explore adjacent nodes.
		for (GOAPNode& adjacent : adjacents) 
		{
			// If the adjacent node isn't in the open list, it is added.
			if (!containsNode(adjacent, openList)) 
			{
				adjacent.setParent(pos);
				adjacent.setH(currentNode.getWorld());
				adjacent.setG(currentNode);
				openList.Push(adjacent);
			}
			// If current path to adjacent node is cheaper than the previous one, the path changes. 
			else if (adjacent.getG() > adjacent.getG() + currentNode.getG()) 
			{
				adjacent.setParent(pos);
				adjacent.setG(currentNode);
			}
		}

		// If open list is empty or the algorithm reach the maximum depth, the plan stops.
		if (openList.Num() == 0 || closedList.Num() > getMaxDepth()) 
		{
			continues = false;
		}
	}

	// Extracts the plan's path in reverse from closed list and copy it to a new variable.
	if (goalReached)
	{
		GOAPNode planNode = lastNode;
		while (!(planNode == startNode))
		{
			sol.Push(planNode.getAction());
			planNode = closedList[planNode.getParent()];
		}
	}
	return sol;
}

void GOAPPlanner::addAction(UGOAPAction* a) 
{
	this->actions.Push(a);
}

GOAPWorldState GOAPPlanner::getGoal() 
{
	return *goal;
}
void GOAPPlanner::setGoal(GOAPWorldState* g) 
{
	this->goal = g;
}
GOAPWorldState GOAPPlanner::getCurrentWorld() 
{
	return *currentWorld;
}
void GOAPPlanner::setCurrentWorld(GOAPWorldState* w) 
{
	this->currentWorld = w;
}

int GOAPPlanner::getMaxDepth() {
	return maxDepth;
}

void GOAPPlanner::setMaxDepth(int md) {
	maxDepth = md;
}