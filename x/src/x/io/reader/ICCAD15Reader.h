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
 * File:   ICCAD15Reader.h
 * Author: jucemar
 *
 * Created on 20 de Fevereiro de 2017, 18:59
 */



#ifndef ICCAD15_READER_H
#define ICCAD15_READER_H

#include "rsyn/engine/Engine.h"

#include "rsyn/core/Rsyn.h"
#include "rsyn/io/reader/ICCAD15Reader.h"
#include "rsyn/model/timing/Timer.h"
#include "rsyn/model/scenario/Scenario.h"
#include "rsyn/model/timing/DefaultTimingModel.h"
#include "rsyn/model/routing/RoutingEstimator.h"

namespace ICCAD15 {

////////////////////////////////////////////////////////////////////////////////
// Reader to ICCAD 2015 benchmarks
////////////////////////////////////////////////////////////////////////////////

class ICCAD15Reader : public Rsyn::ICCAD15Reader {
protected:
		
public:
	ICCAD15Reader() {}
	ICCAD15Reader(const ICCAD15Reader& orig) {}
	virtual ~ICCAD15Reader() {}
	void load(Rsyn::Engine engine, const Rsyn::Json &params);

protected:

	void openBenchmarkFromICCAD15();
	void openBenchmarkFromICCAD15ForGlobalPlacementOnly();
}; // end class 

} // end namespace 
#endif /* RSYN_ICCAD15READER_H */

