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
 
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PhysicalDesignData.h
 * Author: jucemar
 *
 * Created on 12 de Setembro de 2016, 20:03
 */

#ifndef PHYSICALDESIGN_PHYSICALDESIGNDATA_H
#define PHYSICALDESIGN_PHYSICALDESIGNDATA_H

//#include "PhysicalInstanceData.h"

namespace Rsyn {

class PhysicalDesignData : public PhysicalObject {
	friend class PhysicalDesign;
public:
	Rsyn::Design clsDesign;
	Rsyn::Module clsModule;
	
	std::list<std::function<void(Rsyn::Instance cell) >> callbackAddCreatePhysicalCell;
	std::list<std::function<void(Rsyn::Instance cell) >> callbackOnPreCellRemoveEvent;
	Rsyn::Attribute<Rsyn::LibraryPin, PhysicalLibraryPinData> clsPhysicalLibraryPins;
	Rsyn::Attribute<Rsyn::Pin, PhysicalPinData> clsPhysicalPins;
	Rsyn::Attribute<Rsyn::Instance, PhysicalInstanceData> clsPhysicalInstances;
	Rsyn::Attribute<Rsyn::LibraryCell, PhysicalLibraryCellData> clsPhysicalLibraryCells;
	Rsyn::Attribute<Rsyn::Net, PhysicalNetData> clsPhysicalNets;
	Rsyn::List<PhysicalRowData> clsPhysicalRows;
	std::vector<PhysicalSite> clsPhysicalSites;
	std::unordered_map<std::string, int> clsMapPhysicalSites;

	//From LEF file
	std::vector<Rsyn::PhysicalLayer> clsPhysicalLayers;
	std::vector<Rsyn::PhysicalLayer> clsRoutingPhysicalLayers;
	std::vector<Rsyn::PhysicalLayer> clsCutPhysicalLayers;
	std::vector<Rsyn::PhysicalLayer> clsOverlapPhysicalLayers;
	std::unordered_map<std::string, int> clsMapPhysicalLayers;

	std::vector<PhysicalSpacing> clsPhysicalSpacing;

	Rsyn::PhysicalDieData clsPhysicalDie; // total area of the circuit including core bound. 

	DBU clsTotalAreas[NUM_PHYSICAL_TYPES];
	int clsNumElements[NUM_PHYSICAL_TYPES];
	
	DBUxy clsHPWL;
	DBU clsDBUs[NUM_DBU]; // LEF and DEF data base units resolution and DEF/LEF multiplier factor
	
	bool clsLoadDesign : 1;
	bool clsEnablePhysicalPins : 1;
	bool clsEnableMergeRectangles : 1;
	bool clsEnableNetPinBoundaries : 1;
	
	Rsyn::Net clsClkNet;
		
	// Notifications
	std::list<std::tuple<int, PhysicalDesign::PostInstanceMovedCallback>> 
	callbackPostInstanceMoved;
	// todo physicalRow notification

	PhysicalDesignData() : clsClkNet(nullptr), clsDesign(nullptr), clsModule(nullptr) {
		
		clsLoadDesign = false;
		clsEnablePhysicalPins = false;
		clsEnableMergeRectangles = false ;
		clsEnableNetPinBoundaries = false;
		for(int index = 0; index < NUM_DBU; index++){
			clsDBUs[index] = 0;
		} // end for 
		for(int index = 0; index < NUM_PHYSICAL_TYPES; index++) {
			clsTotalAreas[index] = 0.0;
			clsNumElements[index] = 0;
		} // end for  
	} // end constructor 
}; // end class 

} // end namespace

#endif /* PHYSICALDESIGN_PHYSICALDESIGNDATA_H */

