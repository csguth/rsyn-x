/* Copyright 2014-2017 Rsyn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef RSYN_ROUTING_ESTIMATOR_H
#define RSYN_ROUTING_ESTIMATOR_H

#include <iostream>

#include "rsyn/core/Rsyn.h"
#include "rsyn/engine/Service.h"
#include "rsyn/phy/PhysicalDesign.h"
#include "rsyn/model/routing/RCTree.h"
#include "rsyn/model/routing/RoutingEstimationModel.h"
#include "rsyn/model/routing/RoutingExtractionModel.h"
#include "rsyn/model/scenario/Scenario.h"
#include "rsyn/util/Stopwatch.h"

namespace Rsyn {

class Engine;

class RoutingEstimator : public Service, public Rsyn::Observer {
private:

	Rsyn::Design design;
	Rsyn::Module module; // top module
	
	Scenario * clsScenario = nullptr;

	RoutingEstimationModel *routingEstimationModel = nullptr;
	RoutingExtractionModel *routingExtractionModel = nullptr;

	bool clsFullUpdateAlreadyPerformed = false;
	std::set<Rsyn::Net> clsDirtyNets;
	Stopwatch clsStopwatchUpdateSteinerTrees;

	struct RoutingNet {
		RCTree rctree;
		DBU wirelength;
	}; // end struct
	
	DBU clsTotalWirelength;
	
	Rsyn::Attribute<Rsyn::Net, RoutingNet> clsRoutingNets;
	
public:
	
	virtual void start(Engine engine, const Json &params);
	virtual void stop();

	void setRoutingEstimationModel(RoutingEstimationModel *model) { routingEstimationModel = model; }
	void setRoutingExtractionModel(RoutingExtractionModel *model) { routingExtractionModel = model; }

	virtual void
	onPostNetCreate(Rsyn::Net net) override;

	virtual void
	onPreNetRemove(Rsyn::Net net) override;

	virtual void
	onPostCellRemap(Rsyn::Cell cell, Rsyn::LibraryCell oldLibraryCell) override;
	
	Number getLocalWireResPerUnitLength() const { return routingExtractionModel->getLocalWireResPerUnitLength(); }
	Number getLocalWireCapPerUnitLength() const { return routingExtractionModel->getLocalWireCapPerUnitLength(); }
	
	const Rsyn::RCTree &getRCTree(Rsyn::Net net) const { 
		return clsRoutingNets[net].rctree;
	} // end method	
	
	// Timer needs a non-cost tree, for now let's give one to it.
	Rsyn::RCTree &getRCTree(Rsyn::Net net) { 
		return clsRoutingNets[net].rctree;
	} // end method
	
	void updateRoutingOfNet(Rsyn::Net net);
	void updateRoutingFull();
	void updateRouting();
	
	void dirtyInstance(Rsyn::Instance instance) {
		for (Rsyn::Pin pin : instance.allPins()) {
			Rsyn::Net net = pin.getNet();
			if (net)
				clsDirtyNets.insert(net);
		} // end for
	} // end method

	void dirtyNet(Rsyn::Net net) {
		clsDirtyNets.insert(net);
	} // end method
		
	DBU getTotalWirelength() const {
		return clsTotalWirelength;
	} // end method
	
	DBU getNetWirelength(Rsyn::Pin pin, bool skipClock = true) const {
		if (!pin.isConnected())
			return 0;
		return getNetWirelength(pin.getNet(), skipClock);
	} // end method 
	
	DBU getNetWirelength(Rsyn::Net net, bool skipClock = true) const {
		if (net.getNumPins() < 2 || (net == clsScenario->getClockNet() && skipClock))
			return 0;
		const RoutingNet &timingNet = clsRoutingNets[net];
		return timingNet.wirelength;
	} // end method 	

	// TODO: Move the following methods elsewhere.

	// Return the tree node position where a pin is connected to the routing
	// tree.
	DBUxy getSteinerPointer(Rsyn::Net net, Rsyn::Pin pin) const;

	// Return the node index where a pin is connected to the routing tree.
	int getSteinerPointerIndex(const RCTree &rcTree, Rsyn::Pin pin) const;
	
	// Return the index of the node to which the pin connects to the RC tree.
	// Returns -1 if the pin is not connected to the tree.	
	int getRCTreeConnectingNodeIndex(Rsyn::Net net, Rsyn::Pin pin) const { 
		return getRCTreeConnectingNodeIndex(getRCTree(net), pin);
	} // end method
	
	int getRCTreeConnectingNodeIndex(const RCTree &rcTree, Rsyn::Pin pin) const;
	
	// Return the wire capacitance at this node due to the wire segment 
	// connecting this node to its parent.
	Number getWireCapToParentNode(const RCTree &tree, const int nodeIndex) const {
		const RCTree::Node &node = tree.getNode(nodeIndex);
		if (node.propParent == -1)
			return 0;
		
		const RCTreeNodeTag &tagNode = tree.getNodeTag(nodeIndex);
		const RCTreeNodeTag &tagParent = tree.getNodeTag(node.propParent);
		
		const DBU distance = std::abs(tagNode.x - tagParent.x) 
				+ std::abs(tagNode.y - tagParent.y);
		
		return (getLocalWireCapPerUnitLength() * distance) / 2;
	} // end method
	
	// Return the downstream capacitance at a node ignoring the wire capacitance
	// due to the parent (driver) wire.
	EdgeArray<Number> getDownstreamCapacitanceIgnoringParentWire(const RCTree &tree, const int nodeIndex) const {
		const RCTree::Node &node = tree.getNode(nodeIndex);
		return node.getDownstreamCap() - getWireCapToParentNode(tree, nodeIndex);
	} // end method		
	
	void resetRuntime() {
		clsStopwatchUpdateSteinerTrees.reset();
	} // end method
	
	const Stopwatch &getUpdateSteinerTreeRuntime() const { 
		return clsStopwatchUpdateSteinerTrees;
	} // end method
	
}; // end class

} // end namespace

#endif