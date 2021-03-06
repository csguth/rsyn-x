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
 * File:   PhysicalPort.h
 * Author: jucemar
 *
 * Created on 4 de Outubro de 2016, 13:23
 */

#ifndef PHYSICALDESIGN_PHYSICALPORT_H
#define PHYSICALDESIGN_PHYSICALPORT_H


namespace Rsyn {

class PhysicalPort : public PhysicalInstance {
	friend class PhysicalDesign;
protected:
	PhysicalPort(PhysicalInstanceData * data) : PhysicalInstance(data) {}
public:
	PhysicalPort() : PhysicalInstance(nullptr) {}
	PhysicalPort(std::nullptr_t) : PhysicalInstance(nullptr) {}

	// Methods for physical cell when a circuit pin (port) is mapped to it
	// In the Rsyn the circuit pins (ports) and cells are the same object. Therefore,
	// in the mapping to physical object they must be the same object.
	PhysicalLayer getPortLayer() const;
	bool hasPortLayer() const;
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALPORT_H */

