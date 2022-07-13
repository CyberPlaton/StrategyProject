#pragma once

#include "BTNodes.h"
#include "BTBlackboard.h"
#include "BehaviorTree.h"
#include "BTFactory.h"


/*
Example usage:

using namespace bt;
	BTBlackboard blackboard("My Tree Blackboard"); // Create a Blackboard.

	BTFactory factory("My Tree", &blackboard); // Create a Tree with a Global Blackboard, which can be accessed by all Nodes.
	
	factory.Add<BTSequence>("Sequence") // Define the Tree structure.
				.Add<BTRandomSequence>("Random Sequence")
					.Add<BTInverter>("Inverter")
					.End()
				.Add<BTFallback>("Fallback)
	.Build();

	The created structure:

			  "Sequence"
				|    |
			----      -----
		   |			   |
	"Random Sequence"	"Fallback"
		   |
	  ------
	  |
  "Inverter"
*/