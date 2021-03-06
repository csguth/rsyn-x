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
 * File:   PhysicalSpacing.h
 * Author: jucemar
 *
 * Created on 15 de Setembro de 2016, 18:56
 */

#ifndef PHYSICALDESIGN_PHYSICALSPACING_H
#define PHYSICALDESIGN_PHYSICALSPACING_H

namespace Rsyn {

class PhysicalSpacing : public Proxy<PhysicalSpacingData> {
	friend class PhysicalDesign;
	friend class PhysicalDesignData;
protected:
	PhysicalSpacing(PhysicalSpacingData * data) : Proxy(data) {}
public:
	PhysicalSpacing() : Proxy(nullptr) {}
	
	PhysicalSpacing(std::nullptr_t) : Proxy(nullptr) {}
	
	Rsyn::PhysicalLayer getLayer1() const;
	Rsyn::PhysicalLayer getLayer2() const;
	DBU getDistance() const;
}; // end class 

} // end namespace 

#endif /* PHYSICALDESIGN_PHYSICALSPACING_H */

