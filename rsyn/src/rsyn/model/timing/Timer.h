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
 
#ifndef RSYN_TIMER_H_
#define RSYN_TIMER_H_

#include <cmath>
#include <algorithm>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <queue>

#include <ctime>

#include "rsyn/core/Rsyn.h"
#include "rsyn/engine/Service.h"
#include "rsyn/model/timing/EdgeArray.h"

#include "rsyn/util/Stepwatch.h"
#include "rsyn/util/RangeBasedLoop.h"
#include "rsyn/util/dbu.h"
#include "rsyn/util/FloatingPoint.h"

#include "TimingNet.h"
#include "TimingPin.h"
#include "TimingArc.h"
#include "TimingLibraryCell.h"
#include "TimingLibraryPin.h"
#include "TimingLibraryArc.h"
#include "TimingModel.h"
#include "types.h"

// TODO: Remove this dependency
#include "rsyn/io/legacy/ispd13/global.h"

namespace Rsyn {
	
class Engine;
class Scenario;

////////////////////////////////////////////////////////////////////////////////
// Static Timing Analysis
////////////////////////////////////////////////////////////////////////////////

 class Timer : public Service, public Rsyn::Observer {
public:

	enum InputDriverDelayMode {
		// The input driver delay is added as-is to the path delay.
		INPUT_DRIVER_DELAY_MODE_UI_TIMER,

		// The delay added by the input driver is equal to the input driver
		// delay less the input driver delay when driving no load (i.e.
		// parasitic delay).
		INPUT_DRIVER_DELAY_MODE_PRIME_TIME
	}; // end enum

private:
	
	Rsyn::Design design;
	Rsyn::Module module;
	Scenario *clsScenario;
	
	TimingModel *timingModel;
	InputDriverDelayMode inputDriverDelayMode = INPUT_DRIVER_DELAY_MODE_UI_TIMER;
	
public:
	
	virtual void start(Rsyn::Engine engine, const Json &params) override;
	virtual void stop() override;

	virtual void
	onPostInstanceCreate(Rsyn::Instance instance) override;

	virtual void
	onPostCellRemap(Rsyn::Cell cell, Rsyn::LibraryCell oldLibraryCell) override;
	
	////////////////////////////////////////////////////////////////////////////
	// Timing Properties
	////////////////////////////////////////////////////////////////////////////	
private:
	static const bool ENABLE_IITIMER_COMPATIBILITY_MODE;
	static const bool ENABLE_UITIMER_COMPATIBILITY_MODE;
	
	static const std::function<bool(const Number a, const Number b)> 
			TM_MODE_COMPARATORS[NUM_TIMING_MODES];

	static const std::function<bool(const Number a, const Number b)> 
			TM_MODE_REVERSE_COMPARATORS[NUM_TIMING_MODES];
	
	static const std::function<Number(const Number a, const Number b)> 
			TM_MODE_WORST_DELAY_AND_ARRIVAL[NUM_TIMING_MODES];
	
	static const std::function<Number(const Number a, const Number b)> 
			TM_MODE_WORST_REQUIRED[NUM_TIMING_MODES];	
	
	////////////////////////////////////////////////////////////////////////////
	// Rsyn
	////////////////////////////////////////////////////////////////////////////		
public:	
	
	Rsyn::Attribute<Rsyn::Net, TimingNet> clsNetLayer;
	Rsyn::Attribute<Rsyn::Pin, TimingPin> clsPinLayer;
	Rsyn::Attribute<Rsyn::Arc, TimingArc> clsArcLayer;
	Rsyn::Attribute<Rsyn::LibraryArc, TimingLibraryArc> clsLibraryArcLayer;
	Rsyn::Attribute<Rsyn::LibraryPin, TimingLibraryPin> clsLibraryPinLayer;
	Rsyn::Attribute<Rsyn::LibraryCell, TimingLibraryCell> clsLibraryCellLayer;
	
private:
	
	////////////////////////////////////////////////////////////////////////////
	// Parsing
	////////////////////////////////////////////////////////////////////////////		
	
	void parseParams(const std::string &filename);
		
	////////////////////////////////////////////////////////////////////////////
	// Library Methods
	////////////////////////////////////////////////////////////////////////////	

public:

	bool isUnusualTimingArc(const ISPD13::LibParserTimingInfo &libArc) const;

private:

	void initializeTimingCell(
			Rsyn::Cell rsynCell);
	
	void uninitializeTimingCell(
			Rsyn::Cell rsynCell);
	
	void initializeTimingLibraryArc(
			Rsyn::LibraryArc larc,
			const TimingSense sense);
		
	TimingLibraryPin &createTimingLibraryPin(
			Rsyn::LibraryCell rsynLibraryCell, 
			const std::string &name, 
			const bool clocked);
	
	void setTimingLibraryPinHoldConstraint(
			TimingLibraryPin &constrained, 
			TimingLibraryPin &related);
	
	void setTimingLibraryPinSetupConstraint(
			TimingLibraryPin &constrained, 
			TimingLibraryPin &related);	
	
public:

	inline TimingLibraryCell &getTimingLibraryCell(Rsyn::LibraryCell rsynLibraryCell) { return clsLibraryCellLayer[rsynLibraryCell]; }
	inline const TimingLibraryCell &getTimingLibraryCell(Rsyn::LibraryCell rsynLibraryCell) const { return clsLibraryCellLayer[rsynLibraryCell]; }
	
	inline TimingLibraryPin &getTimingLibraryPin(Rsyn::LibraryPin rsynLibraryPin) { return clsLibraryPinLayer[rsynLibraryPin]; }
	inline const TimingLibraryPin &getTimingLibraryPin(Rsyn::LibraryPin rsynLibraryPin) const { return clsLibraryPinLayer[rsynLibraryPin]; }
	
	inline TimingLibraryPin &getTimingLibraryPin(Rsyn::Pin rsynPin) { return getTimingLibraryPin(rsynPin.getLibraryPin()); }
	inline const TimingLibraryPin &getTimingLibraryPin(Rsyn::Pin rsynPin) const { return getTimingLibraryPin(rsynPin.getLibraryPin()); }
	
	inline TimingPin &getTimingPin(Rsyn::Pin rsynPin) { return clsPinLayer[rsynPin]; }
	inline const TimingPin &getTimingPin(Rsyn::Pin rsynPin) const { return clsPinLayer[rsynPin]; }
	
	inline TimingNet &getTimingNet(Rsyn::Net rsynNet) { return clsNetLayer[rsynNet]; }
	inline const TimingNet &getTimingNet(Rsyn::Net rsynNet) const { return clsNetLayer[rsynNet]; }	
	
	inline TimingArc &getTimingArc(Rsyn::Arc rsynArc) { return clsArcLayer[rsynArc]; }
	inline const TimingArc &getTimingArc(Rsyn::Arc rsynArc) const { return clsArcLayer[rsynArc]; }
	
	inline TimingLibraryArc &getTimingLibraryArc(Rsyn::LibraryArc rsynLibraryArc) { return clsLibraryArcLayer[rsynLibraryArc]; }
	inline const TimingLibraryArc &getTimingLibraryArc(Rsyn::LibraryArc rsynLibraryArc) const { return clsLibraryArcLayer[rsynLibraryArc]; }

	inline TimingLibraryArc &getTimingLibraryArc(Rsyn::Arc rsynArc) { return getTimingLibraryArc(rsynArc.getLibraryArc()); }
	inline const TimingLibraryArc &getTimingLibraryArc(Rsyn::Arc rsynArc) const { return getTimingLibraryArc(rsynArc.getLibraryArc()); }
	
	inline TimingPin &getFromTimingPinOfArc(Rsyn::Arc rsynArc) { return getTimingPin(rsynArc.getFromPin()); }
	inline const TimingPin &getFromTimingPinOfArc(Rsyn::Arc rsynArc) const { return getTimingPin(rsynArc.getFromPin()); }

	inline TimingPin &getToTimingPinOfArc(Rsyn::Arc rsynArc) { return getTimingPin(rsynArc.getToPin()); }
	inline const TimingPin &getToTimingPinOfArc(Rsyn::Arc rsynArc) const { return getTimingPin(rsynArc.getToPin()); }

	inline bool isNonUnate(Rsyn::LibraryArc larc) { return getTimingLibraryArc(larc).sense  == NON_UNATE; }
	inline bool isNonUnate(Rsyn::Arc arc) { return isNonUnate(arc.getLibraryArc()); }
	
private:	
	
	////////////////////////////////////////////////////////////////////////////
	// Timing
	////////////////////////////////////////////////////////////////////////////
	
	bool clsForceFullTimingUpdate;
	
	int clsSign;
	int generateNextSign() { return ++clsSign; /*must be pre-increment*/ }
	int getSign() const { return clsSign; }	
	
	Number clsTNS[NUM_TIMING_MODES]; // total negative slack (only accounts for the worst edge at each endpoint)
	Number clsWNS[NUM_TIMING_MODES]; // worst negative slack (only accounts for the worst edge at each endpoint)
	Number clsWorstSlack[NUM_TIMING_MODES]; // worst slack (may be positive, only accounts for the worst edge at each endpoint)
	Number clsMaxArrivalTime[NUM_TIMING_MODES]; // max arrival at any endpoint
	Number clsMinArrivalTime[NUM_TIMING_MODES]; // min arrival at any endpoint
	int clsNumCriticalEndpoints[NUM_TIMING_MODES];
	pair<Rsyn::Pin, TimingTransition> clsCriticalPathEndpoint[NUM_TIMING_MODES];	
	Number clsSlackChecksum;
	std::vector<Rsyn::Pin> clsCriticalEndpoints[NUM_TIMING_MODES];
	
	Number clsMaxCentrality[NUM_TIMING_MODES];
		
	std::set<Rsyn::Cell> sequentialCells;
	std::set<Rsyn::Pin> endpoints;
	std::set<Rsyn::Pin> ties;
	std::set<Rsyn::Pin> floatingEndpoints;
	std::set<Rsyn::Pin> floatingStartpoints;
	
	std::set<Rsyn::Net> dirtyNets;
	std::set<Rsyn::Instance> clsDirtyTimingCells;	
	
	void timingBuildTimingArcs_SetupBacktrackEdge(
			TimingArc &arc, 
			const TimingSense sense);
	
	////////////////////////////////////////////////////////////////////////////
	// Update Timing
	////////////////////////////////////////////////////////////////////////////
	
	void updateTiming_Arc_NonUnate(const TimingMode mode, const EdgeArray<Number> islew, const EdgeArray<Number> load, Rsyn::Arc arc);
	void updateTiming_Arc(const TimingMode mode, const EdgeArray<Number> islew, const EdgeArray<Number> load, const bool skip, Rsyn::Arc arc, Rsyn::LibraryArc larc, TimingArcState &state);
	
	void updateTiming_Net_InitDriver(Rsyn::Pin driver, const TimingMode mode, const EdgeArray<Number> load);
	void updateTiming_Net_TimingMode(const TimingMode mode, Rsyn::Net net, const EdgeArray<Number> load, Rsyn::Arc arc);
	void updateTiming_Net(Rsyn::Net net);

	void updateTiming_HandleFloatingPins();
	
	// Propagate arrival times.
	void updateTiming_PropagateArrivalTimes();
	void updateTiming_PropagateArrivalTimesIncremental(std::set<Rsyn::Net> &endpoints);		
	
	// Update requited time at endpoints.
	void updateTiming_UpdateTimingTests_SetupHold_DataPin(Rsyn::Pin pin);
	void updateTiming_UpdateTimingTests();

	// Update timing violations (i.e. TNS, WNS).
	void updateTiming_UpdateTimingViolations();
	
	// Propagate required times.
	void updateTiming_PropagateRequiredTimes_Net(Rsyn::Net net);
	void updateTiming_PropagateRequiredTimes();
	void updateTiming_PropagateRequiredTimesIncremental(const std::set<Rsyn::Net> &nets);

	// Propagate endpoint's criticalities to compute centralities.
	void updateTiming_Centrality_Net(Rsyn::Net net);
	void updateTiming_Centrality();
	void updateTiming_CentralityIncremental(const std::set<Rsyn::Net> &nets);
	
	// Update the sorted list of critical endpoints.
	void updateTiming_CriticalEndpoints();
		
	// Indicates if the state of a pin has changed significantly in terms of
	// arrival time propagation.
	bool hasStateChangedSignificantlyForArrivalTimePropagation(
			const std::array<TimingPinState, NUM_TIMING_MODES> &state0, 
			const std::array<TimingPinState, NUM_TIMING_MODES> &state1, 
			const Number precision = 1e-6f
	) const {
		bool changed = false;
		for (const TimingMode mode : allTimingModes()) {
			for (const TimingTransition transition : allTimingTransitions()) {
				changed = changed || !FloatingPoint::approximatelyEqual(
					state0[mode].slew[transition], state1[mode].slew[transition], precision);
				changed = changed || !FloatingPoint::approximatelyEqual(
					state0[mode].a[transition], state1[mode].a[transition], precision);			
			} // end for
		} // end for
		return changed;
	} // end method

	// Indicates if the state of a pin has changed significantly in terms of
	// require time propagation.
	bool hasStateChangedSignificantlyForRequiredTimePropagation(
			const std::array<TimingPinState, NUM_TIMING_MODES> &state0, 
			const std::array<TimingPinState, NUM_TIMING_MODES> &state1, 
			const Number precision = 1e-6f
	) const {
		bool changed = false;
		for (const TimingMode mode : allTimingModes()) {
			for (const TimingTransition transition : allTimingTransitions()) {
				changed = changed || !FloatingPoint::approximatelyEqual(
					state0[mode].q[transition], state1[mode].q[transition], precision);			
			} // end for
		} // end for
		return changed;
	} // end method
	
	////////////////////////////////////////////////////////////////////////////
	// Utilities
	////////////////////////////////////////////////////////////////////////////
	
	TimingSense getTimingSenseFromString(const std::string &sense) {
		if (sense == "non_unate") return NON_UNATE;
		if (sense == "negative_unate") return NEGATIVE_UNATE;
		if (sense == "positive_unate") return POSITIVE_UNATE;
		return TIMING_SENSE_INVALID;
	} // end function	
	
	////////////////////////////////////////////////////////////////////////////
	// Initialization
	////////////////////////////////////////////////////////////////////////////

public:	

	void init(
			Rsyn::Design rsynDesign,
			Rsyn::Engine enginePtr,
			Scenario * scenario,
			const ISPD13::LIBInfo &libEarly,
			const ISPD13::LIBInfo &libLate);
		
	////////////////////////////////////////////////////////////////////////////
	// Timing state
	////////////////////////////////////////////////////////////////////////////
	
public:	

	// Setup timing model.
	void setTimingModel(TimingModel *model) {
		timingModel = model;
		clsForceFullTimingUpdate = true;
	} // end method

	// Setup the input driver delay mode.
	void setInputDriverDelayMode(const InputDriverDelayMode mode) {
		inputDriverDelayMode = mode;
	} // end method

	// Mark an instance as needing timing update.
	void dirtyInstance(Rsyn::Instance instance) { clsDirtyTimingCells.insert(instance); }
	
	// Mark an instance as needing timing update.
	void dirtyNet(Rsyn::Net net) { dirtyNets.insert(net); }
	
	// Update timing.
	void updateTimingFull();
	
	// Update timing incrementally.
	void updateTimingIncremental();
	
	// Update timing of a single net. No timing propagation.
	void updateTimingOfNet(Rsyn::Net net);
	
	// Update timing of nets connected to a cell. Does not propagate timing, 
	// but the timing of driving nets are computed before the timing of the
	// sink nets. This method does not update the Steiner trees automatically.
	void updateTimingLocally(Rsyn::Instance cell);
		
	// Compute slack according to the timing mode.
	Number computeSlack(const TimingMode mode, const Number arrival, const Number required) const {
		switch (mode) {
			case LATE : return required - arrival;
			case EARLY: return arrival - required;
			default: assert(false);
		} // end switch		
	} // end method

	// Returns the current timing model.
	TimingModel *getTimingModel() {
		return timingModel;
	} // end method
	
	// [TODO] Make these get methods const...
	
	Number getLibraryPinInputCapacitance(Rsyn::LibraryPin lpin) const { 
		return timingModel->getLibraryPinInputCapacitance(lpin);
	} // end method
	
	Number getPinInputCapacitance(Rsyn::Pin pin) const { 
		return timingModel->getPinInputCapacitance(pin);
	} // end method

	Number getPinLoadCapacitance(Rsyn::Pin pin, const TimingTransition oedge) const { 
		Rsyn::Net net = pin.getNet();
		return net? getNetLoad(net)[oedge] : 0; 
	} // end method
		
	Number getPinArrivalTime(const TimingPin &timingPin, const TimingMode mode, const TimingTransition transition) const {
		return timingPin.state[mode].a[transition];
	} // end method
	
	Number getPinArrivalTime(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const {
		return getPinArrivalTime(getTimingPin(pin), mode, transition);
	} // end method
	
	Number getPinWorstArrivalTime(const TimingPin &timingPin, const TimingMode mode) const {
		const Number fall = getPinArrivalTime(timingPin, mode, FALL);
		const Number rise = getPinArrivalTime(timingPin, mode, RISE);
		return TM_MODE_WORST_DELAY_AND_ARRIVAL[mode](fall, rise);
	} // end method	
	
	Number getPinWorstArrivalTime(Rsyn::Pin pin, const TimingMode mode) const {
		return getPinWorstArrivalTime(getTimingPin(pin), mode);
	} // end method
	
	Number getPinWireDelay(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const {
		return getPinWireDelay(getTimingPin(pin), mode, transition);
	} // end method 

	Number getPinWireDelay(const TimingPin &timingPin, const TimingMode mode, const TimingTransition transition) const {
		return timingPin.state[mode].wdelay[transition];
	} // end method 
	
	Number getPinRequiredTime(const TimingPin &timingPin, const TimingMode mode, const TimingTransition transition) const {
		return timingPin.state[mode].q[transition];
	} // end method
	
	Number getPinRequiredTime(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const {
		return getPinRequiredTime(getTimingPin(pin), mode, transition);
	} // end method
	
	Number getPinWorstRequiredTime(const TimingPin &timingPin, const TimingMode mode) const {
		const Number fall = getPinRequiredTime(timingPin, mode, FALL);
		const Number rise = getPinRequiredTime(timingPin, mode, RISE);
		return TM_MODE_WORST_REQUIRED[mode](fall, rise);
	} // end method	
	
	Number getPinWorstRequiredTime(Rsyn::Pin pin, const TimingMode mode) const {
		return getPinWorstRequiredTime(getTimingPin(pin), mode);
	} // end method	
	
	Number getPinSlack(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const {
		return getTimingPin(pin).getSlack(mode, transition);
	} // end method
	
	Number getPinSlack(const TimingPin &timingPin, const TimingMode mode, const TimingTransition transition) const {
		return timingPin.getSlack(mode, transition);
	} // end method	
	
	Number getPinNegativeSlack(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const {
		return getPinNegativeSlack(getTimingPin(pin), mode, transition);
	} // end method 
	
	Number getPinNegativeSlack(const TimingPin &timingPin, const TimingMode mode, const TimingTransition transition) const {
		return timingPin.getNegativeSlack(mode, transition);
	} // end method 
	
	Number getPinSlew(const TimingPin &timingPin, const TimingMode mode, const TimingTransition transition) const {
		return timingPin.state[mode].slew[transition];
	} // end method		
	
	Number getPinSlew(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const {
		return getPinSlew(getTimingPin(pin), mode, transition);
	} // end method	
	
	// Returns the (adjusted) clock period of the worst path passing through a
	// pin. The adjusted clock period is the clock period accounting for skew,
	// setup/hold time, etc.	
	Number getPinAdjustedClockPeriod(const TimingPin &timingPin, const TimingMode mode, const TimingTransition transition) const {
		return timingPin.state[mode].wsq[transition];
	} // end method		
	
	Number getPinAdjustedClockPeriod(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const {
		return getPinAdjustedClockPeriod(getTimingPin(pin), mode, transition);
	} // end method
	
	Number getTns(const TimingMode mode) const {
		return clsTNS[mode];
	} // end method
	
	Number getWns(const TimingMode mode) const {
		return clsWNS[mode];
	} // end method
	
	Number getMaxArrivalTime(const TimingMode mode) const {
		return clsMaxArrivalTime[mode];
	} // end method

	Number getMinArrivalTime(const TimingMode mode) const {
		return clsMinArrivalTime[mode];
	} // end method
	
	int getNumCriticalEndpoints(const TimingMode mode) const {
		return clsNumCriticalEndpoints[mode];
	} // end method 

	// Gets the clock net.
	Rsyn::Net getClockNet() const;
	Number getClockPeriod() const;
	
	std::tuple<Number, TimingTransition> 
	getPinWorstSlackWithTransition(const TimingPin &timingPin, const TimingMode mode) const {
		const Number riseSlack = timingPin.getSlack(mode, RISE);
		const Number fallSlack = timingPin.getSlack(mode, FALL);
		return (riseSlack < fallSlack)?
			std::make_tuple(riseSlack, RISE) :
			std::make_tuple(fallSlack, FALL);
	} // end method
	
	std::tuple<Number, TimingTransition> 
	getPinWorstSlackWithTransition(Rsyn::Pin pin, const TimingMode mode) const {
		const TimingPin &timingPin = getTimingPin(pin);
		return getPinWorstSlackWithTransition(timingPin, mode);
	} // end method	
	
	// Returns the critical arc of a cell. Note that this arc not necessarily
	// is the slowest (late) / fastest (early), but it is the arc related to
	// the critical path passing thru the cell.
	// The output is a tuple (arc, input transition, output transition).
	std::tuple<Rsyn::Arc, TimingTransition, TimingTransition>
	getCellCriticalArc(Rsyn::Instance cell, const TimingMode mode) const {
		Number criticalSlack = +std::numeric_limits<Number>::infinity();
		Rsyn::Arc criticalArc = nullptr;
		TimingTransition criticalInputTransition;
		TimingTransition criticalOutputTransition;
				
		for (Rsyn::Arc arc : cell.allArcs()) {
			const TimingArc &timingArc = getTimingArc(arc);
			const TimingPin &from = getTimingPin(arc.getFromPin());
			
			for (const TimingTransition oedge : allTimingTransitions()) {
				const TimingTransition iedge = timingArc.state[mode].backtrack[oedge];
				const Number slack = getPinSlack(from, mode, iedge);
				if (slack < criticalSlack) {
					criticalSlack = slack;
					criticalArc = arc;
					criticalInputTransition = iedge;
					criticalOutputTransition = oedge;
				} // end if
			} // end for
		} // end for
		
		return std::make_tuple(criticalArc,
				criticalInputTransition, criticalOutputTransition);
	} // end method
	
	// Returns the percentage of the worst path delay passing through this cell
	// due to this cell. For instance, if the worst path delay is 100 and the
	// cell delay is 25, returns 0.25 (i.e. 25% of the path delay is due to
	// the cell).
	Number getCellWorstPercentageDelay(const Rsyn::Instance cell, const TimingMode mode) const {
		const std::tuple<Rsyn::Arc, TimingTransition, TimingTransition> t = 
				getCellCriticalArc(cell, mode);
		
		Rsyn::Arc arc = std::get<0>(t);
		if (arc) {
			const TimingTransition oedge = std::get<2>(t);
			return getArcWorstPercentageDelay(arc, mode, oedge);
		} else {
			return 0;
		} // end else
	} // end method
	
	Number getCellWorstNegativeSlack(const Rsyn::Instance cell, const TimingMode mode) const {
		Number wns = 0;
		for(Rsyn::Pin pin : cell.allPins(Rsyn::OUT)){
			Number wnsPin = getPinWorstNegativeSlack(pin, mode);
			if(wnsPin < wns)
				wns = wnsPin;
		} // end for 
		return wns;
	} // end method 
	
	Number getCellWorstSlack(const Rsyn::Instance cell, const TimingMode mode) const {
		Number wns = std::numeric_limits<Number>::max() ;
		for(Rsyn::Pin pin : cell.allPins(Rsyn::OUT)){
			Number wnsPin = getPinWorstSlack(pin, mode);
			if(wnsPin < wns)
				wns = wnsPin;
		} // end for 
		return wns;
	} // end method 

	Number getDriverWorstSlack(const Rsyn::Instance cell, const TimingMode mode) const {
		Number slack = std::numeric_limits<Number>::max();
		for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) {
			if (!pin.isConnected() /*|| pin.isClocked()*/)
				continue;
			Rsyn::Net net = pin.getNet();
			if (net.getNumPins() < 2)
				continue;
			Rsyn::Pin driver = net.getAnyDriver();
			Number pinSlack = getPinWorstSlack(driver, mode);
			slack = std::min(slack, pinSlack);
		} // end for 
		return slack;
	} // end method 

	Rsyn::Pin getDriverPinWorstSlack(const Rsyn::Instance cell, const TimingMode mode) const {
		Number slack = std::numeric_limits<Number>::max();
		Rsyn::Pin wsPin = nullptr;
		for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) {
			if (!pin.isConnected() /*|| pin.isClocked()*/)
				continue;
			Rsyn::Net net = pin.getNet();
			if (net.getNumPins() < 2)
				continue;
			Rsyn::Pin driver = net.getAnyDriver();
			Number pinSlack = getPinWorstSlack(driver, mode);
			if (pinSlack < slack) {
				slack = pinSlack;
				wsPin = driver;
			} // end if 
		} // end for 
		return wsPin;
	} // end method 
	
	// Returns the worst slack at this pin.
	Number getPinWorstSlack(const TimingPin &timingPin, const TimingMode mode, const Number uninit = UNINITVALUE) const {
		const bool fall = isTimingAsserted(timingPin, mode, FALL);
		const bool rise = isTimingAsserted(timingPin, mode, RISE);
		
		if (fall && rise) {
			return timingPin.getWorstSlack(mode);
		} else if (fall) {
			return timingPin.getSlack(mode, FALL);
		} else if (rise) {
			return timingPin.getSlack(mode, RISE);
		} else {
			return uninit;
		} // end else
	} // end method
	
	Number getPinWorstSlack(Rsyn::Pin pin, const TimingMode mode, const Number uninit = UNINITVALUE) const {
		return getPinWorstSlack(getTimingPin(pin), mode, uninit);
	} // end method	

	// Returns the worst negative slack at this pin.
	Number getPinWorstNegativeSlack(const TimingPin &timingPin, const TimingMode mode, const Number uninit = UNINITVALUE) const {
		return std::min((Number) 0, getPinWorstSlack(timingPin, mode, uninit));
	} // end method
	
	Number getPinWorstNegativeSlack(Rsyn::Pin pin, const TimingMode mode, const Number uninit = UNINITVALUE) const {
		return getPinWorstNegativeSlack(getTimingPin(pin), mode, uninit);
	} // end method		
	
	// Returns the worst slack at this pin. If the timing is not asserted 
	// (defined) in this pin, returns the default value passed as argument.
	Number getPinWorstSlackSafe(const TimingPin &timingPin, const TimingMode mode, const Number defaultSlack) const {
		const Number slack = getPinWorstSlack(timingPin, mode);
		return isUninitializedValue(slack)? defaultSlack : slack;
	} // end method

	Number getPinWorstSlackSafe(Rsyn::Pin pin, const TimingMode mode, const Number defaultSlack) const {
		return getPinWorstSlackSafe(getTimingPin(pin), mode, defaultSlack);
	} // end method	
	
	// Returns the delay of the worst path passing through a pin.
	Number getPinWorstPathDelay(const TimingPin &timingPin, const TimingMode mode, const TimingTransition edge) const {
		if (isTimingAsserted(timingPin, mode, edge)) {
			const Number t = timingPin.state[mode].wsq[edge]; // adjusted clock period
			const Number a = timingPin.state[mode].a[edge];   // arrival
			const Number q = timingPin.state[mode].q[edge];   // required				
			return a + (t - q);	
		} else {
			return 0;
		} // end method
	} // end method
	
	Number getPinWorstPathDelay(Rsyn::Pin pin, const TimingMode mode, const TimingTransition edge) const {
		return getPinWorstPathDelay(getTimingPin(pin), mode, edge);
	} // end method
	
	// [TODO] Remove this function. Use getPinWorstSlackWithTransition() 
	// instead.	
	TimingTransition getPinTimingTransition(Rsyn::Pin pin, const TimingMode mode) const {
		switch (mode) {
			case LATE: if(getPinRequiredTime(pin, mode, RISE) - getPinArrivalTime(pin, mode, RISE) < 
				getPinRequiredTime(pin, mode, FALL) - getPinArrivalTime(pin, mode, FALL))
					return RISE;
				return FALL;
			case EARLY: if(getPinArrivalTime(pin, mode, RISE) - getPinRequiredTime(pin, mode, RISE) <
				getPinArrivalTime(pin, mode, FALL) - getPinRequiredTime(pin, mode, FALL))
					return RISE;
				return FALL;
			default: assert(false);
		} // end switch
	} // end method 
	
	Number getPinSensitivity(Rsyn::Pin pin, const TimingMode mode) const;
	Number getPinSensitivity(Rsyn::Pin pin, const TimingMode mode, const TimingTransition transition) const;

	// Returns the percentage of the worst path delay passing through an arc
	// due to the arc. For instance, if the worst path delay is 100 and the
	// arc delay is 25, returns 0.25 (i.e. 25% of the path delay is due to
	// the cell).
	Number getArcWorstPercentageDelay(Rsyn::Arc arc, const TimingMode mode, const TimingTransition oedge) const {
		Rsyn::Pin to = arc.getToPin();
		const Number arcDelay = getArcDelay(arc, mode, oedge);
		const Number pathDelay = getPinWorstPathDelay(to, mode, oedge);
		return pathDelay != 0? arcDelay / pathDelay : 0;
	} // end method	
	
	Number getArcDelay(const TimingArc &timingArc, const TimingMode mode, const TimingTransition oedge) const {
		return timingArc.state[mode].delay[oedge];
	} // end method	
	
	Number getArcDelay(Rsyn::Arc arc, const TimingMode mode, const TimingTransition oedge) const {
		return getArcDelay(getTimingArc(arc), mode, oedge);
	} // end method

	Number getArcInputSlew(Rsyn::Arc arc, const TimingMode mode, const TimingTransition iedge) const {
		const TimingPin &from = getTimingPin(arc.getFromPin());
		return from.state[mode].slew[iedge];
	} // end method		
	
	Number getArcInputSlewWithRespecToOutputTransition(Rsyn::Arc arc, const TimingMode mode, const TimingTransition oedge) const {
		const TimingArc &timingArc = getTimingArc(arc);
		const TimingPin &from = getTimingPin(arc.getFromPin());
		
		return from.state[mode].slew[timingArc.state[mode].backtrack[oedge]];
	} // end method	
	
	Number getArcOutputSlew(Rsyn::Arc arc, const TimingMode mode, const TimingTransition oedge) const {
		return getTimingArc(arc).state[mode].oslew[oedge];
	} // end method
		
	Number getArcGain(Rsyn::Arc arc, const TimingMode mode, const TimingTransition oedge) const {
		Rsyn::Pin from = arc.getFromPin();
		Rsyn::Pin to = arc.getToPin();
		return getPinLoadCapacitance(to, oedge) / getPinInputCapacitance(from);
	} // end method	
	
	Number getArcInputWireDelay(Rsyn::Arc arc, const TimingMode mode, const TimingTransition iedge) const {
		const TimingPin &from = getTimingPin(arc.getFromPin());
		return from.state[mode].wdelay[iedge];
	} // end method

	// Find the sink with most similar slack to the "to" pin of the arc and
	// returns the wire delay to that pin.
	Number getArcOutputWireDelay(Rsyn::Arc arc, const TimingMode mode, const TimingTransition oedge) const {
		Rsyn::Net net = arc.getToNet();
		if (!net)
			return 0;
		
		const TimingPin &from = getTimingPin(arc.getFromPin());
		const Number targetSlack = getPinSlack(from, mode, oedge);
		
		Number smallestError = +std::numeric_limits<Number>::infinity();
		Number wireDelay = 0;
		
		for (Rsyn::Pin pin : net.allPins(Rsyn::SINK)) {
			const TimingPin &timingPin = getTimingPin(pin);
			const Number slack = getPinSlack(timingPin, mode, oedge);
			const Number error = std::abs(slack - targetSlack);
			
			if (error < smallestError) {
				smallestError = error;
				wireDelay = timingPin.state[mode].wdelay[oedge];
			} // end if
		} // end for
		
		return wireDelay;
	} // end method	
	
	// Returns the max wire delay to input pins of this cell.
	Number getCellMaxInputWireDelay(Rsyn::Instance cell, const TimingMode mode) const {
		Number maxWireDelay = 0;
		for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) {
			const TimingPin &timingPin = getTimingPin(pin);
			maxWireDelay = std::max(maxWireDelay, timingPin.state[mode].wdelay.getMax());
		} // end for
		return maxWireDelay;
	} // end method
	
	// Returns the max wire delay from one cell's output to its sinks.
	Number getCellMaxOutputWireDelay(Rsyn::Instance cell, const TimingMode mode) const {
		Number maxWireDelay = 0;
		for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
			Rsyn::Net net = pin.getNet();
			if (net) {
				for (Rsyn::Pin sink : net.allPins(Rsyn::SINK)) {
					const TimingPin &timingPin = getTimingPin(sink);
					maxWireDelay = std::max(maxWireDelay, timingPin.state[mode].wdelay.getMax());
				} // end for
			} // end if
		} // end for
		return maxWireDelay;
	} // end method		
	
	// Returns the arrival time propagated to the "to" pin of an arc. Note that
	// arrival time may be different than the actual arrival time stored at "to"
	// since "to" stores the worst arrival time.
	Number getArcArrivalTimeAtToPin(Rsyn::Arc arc, const TimingMode mode, const TimingTransition oedge) const {
			const TimingPin &from = getTimingPin(arc.getFromPin());
			const TimingArc &timingArc = getTimingArc(arc);

			return timingArc.state[mode].delay[oedge]
					+ from.state[mode].a[timingArc.state[mode].backtrack[oedge]];
	} // end method

	Number getArcWorstArrivalTimeAtToPin(Rsyn::Arc arc, const TimingMode mode) {
		const Number fall = getArcArrivalTimeAtToPin(arc, mode, FALL);
		const Number rise = getArcArrivalTimeAtToPin(arc, mode, RISE);
		return TM_MODE_WORST_DELAY_AND_ARRIVAL[mode](fall, rise);
	} // end method

	// Returns the slack criticality i.e. the ratio of the slack and wns. It
	// should be in the range [0, 1] if timing is up-to-date.
	Number getCriticality(const Number slack, const TimingMode mode) const {
		const Number wns = getWns(mode);
		const Number nslack = std::min(0.0f, slack);
		return wns < 0 ?
			std::max((Number)0, std::min((Number)1, nslack / wns)) :
			0;
	} // end method	

	// Returns the pin criticality i.e. the ratio of this pin slack and wns. It
	// should be in the range [0, 1] if timing is up-to-date.
	Number getPinCriticality(Rsyn::Pin pin, const TimingMode mode) const {
		return getCriticality(getPinWorstSlack(pin, mode), mode);
	} // end method
	
	Number getPinCriticality(const TimingPin &timingPin, const TimingMode mode) const {
		return getCriticality(getPinWorstSlack(timingPin, mode), mode);
	} // end method	
	
	// Returns the cell criticality i.e. the worst output pin criticality for
	// combinational cells or the worst criticality among the data and 
	// output pins of flip-flops.
	// It should be in the range [0, 1] if timing is up-to-date.
	Number getCellCriticality(Rsyn::Instance cell, const TimingMode mode) const {
		Number criticality = 0;	
		for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
			criticality = std::max(criticality, getPinCriticality(pin, mode));
		} // end for
				
		if (cell.isSequential()) {
			Rsyn::Pin dpin = getDataPin(cell);
			if (dpin) {
				criticality = std::max(criticality, getPinCriticality(dpin, mode));
			} // end if
		} // end if
		
		return criticality;	
	} // end method
	
	// Returns the net criticality i.e. the worst driver pin criticality.
	// It should be in the range [0, 1] if timing is up-to-date.
	Number getNetCriticality(Rsyn::Net net, const TimingMode mode) const {
		Number criticality = 0;
		for (Rsyn::Pin pin : net.allPins(Rsyn::DRIVER)) {
			criticality = std::max(criticality, getPinCriticality(pin, mode));
		} // end for
		return criticality;	
	} // end method	
		
	// [TODO] Should not be inside timer, too specific.
	Number getSmoothedCriticality(Rsyn::Pin pin, const TimingMode mode) const {	
		const Number medianCriticality = getPinCriticality(
				clsCriticalEndpoints[mode][clsNumCriticalEndpoints[mode]/2], mode);
		
		const Number pinCriticality = getPinCriticality(pin, mode);

		if (FloatingPoint::approximatelyZero(pinCriticality))
			return 0.0f;
		else if (FloatingPoint::approximatelyEqual( pinCriticality, medianCriticality))
			return 0.5f;
		else if ( pinCriticality > medianCriticality ) 
			return 0.5f * ( 1.0f + std::sqrt(
									( pinCriticality - medianCriticality ) / 
									( 1 - medianCriticality) ) ); 
		else 
			return 0.5f * ( 1.0f - std::sqrt(
									( medianCriticality - pinCriticality ) / 
									( medianCriticality) ) ); 
	} // end method
	
	// Returns the pin relativity.
	//
	// The relativity indicates how many times the delay of the worst path 
	// passing through a pin is longer (shorter for early mode) than the 
	// adjusted clock period (clock period accounting for skew, setup/hold time,
	// etc).
	//
	// Note that the worst path passing through a pin is not necessarily
	// critical.
	//
	// By definition, the relativity is always greater or equal to than zero.
	//
	// For a pin p:
	//   relativity(p) <= 1 if p is non-critical
	//   relativity(p) >  1 if p is critical
	//   
	//   relativity(p) =  0 if and only if p is part of unconnected logic.
	//
	// For instance, for late mode, if the delay of worst path passing through a 
	// pin is 10 and the clock period is 5, the relativity is 2. Similarly, 
	// if the path delay is 2 and the clock period is 5, the relativity is
	// 0.4.
	
	Number getPinRelativity(const TimingPin &timingPin, const TimingMode mode) const {
		EdgeArray<Number> relativity; 
		for (const TimingTransition edge : allTimingTransitions()) {
			const Number t = timingPin.state[mode].wsq[edge];   // adjusted clock period
			if (t > 0 && isTimingAsserted(timingPin, mode, edge)) {
				const Number a = timingPin.state[mode].a[edge]; // arrival
				const Number q = timingPin.state[mode].q[edge]; // required				
				relativity[edge] = (a + (t - q)) / t;
			} else {
				relativity[edge] = 0;
			} // end if
		} // end for
		
		switch (mode) {
			case EARLY: return relativity.getMin() > 0 ? 1/relativity.getMin() : 0;
			case LATE : return relativity.getMax();
			default: exit(1);
		} // end switch
	} // end method		

	Number getPinRelativity(Rsyn::Pin pin, const TimingMode mode) const {
		return getPinRelativity(getTimingPin(pin), mode);
	} // end method
	
	// Returns the cell relativity i.e. the worst output pin relativity for
	// combinational cells or the worst relativity among the data and 
	// output pins of flip-flops.
	Number getCellRelativity(Rsyn::Instance cell, const TimingMode mode) const {
		Number relativity = -std::numeric_limits<Number>::infinity();

		bool flag = false;
		for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
			relativity = std::max(relativity, getPinRelativity(pin, mode));
			flag = true;
		} // end for
				
		if (cell.isSequential()) {
			Rsyn::Pin dpin = getDataPin(cell);
			if (dpin) {
				relativity = std::max(relativity, getPinRelativity(dpin, mode));
				flag = true;
			} // end if
		} // end if
		
		return flag? relativity : 0;	
	} // end method	
	
	// Returns the net relativity i.e. the worst driver pin relativity.
	Number getNetRelativity(Rsyn::Net net, const TimingMode mode) const {
		Number relativity = -std::numeric_limits<Number>::infinity();
		bool flag = false;
		for (Rsyn::Pin pin : net.allPins(Rsyn::DRIVER)) {
			relativity = std::max(relativity, getPinRelativity(pin, mode));
			flag = true;
		} // end for
		return flag? relativity : 0;	
	} // end method		
	
	// [TODO]
	Number getPinCentrality(Rsyn::Pin pin, const TimingMode mode) const {
		if (clsMaxCentrality[mode] > 0) {
			const TimingPin &timingPin = getTimingPin(pin);
			return timingPin.state[mode].centrality / clsMaxCentrality[mode];
		} else {
			return 0;
		} // end else
	} // end method

	// [TODO]
	Number getCellCentrality(Rsyn::Instance cell, const TimingMode mode) const {
		Number centrality = 0;
		// For combinational cells, we would only need to traverse output pins,
		// but for sequential we also need to check the data pin. Since,
		// for combinational cells, the centrality of an input pin is always
		// less or equal than the output pin, traversing all pins is ok.
		for (Rsyn::Pin pin : cell.allPins()) {
			centrality = std::max(centrality, getPinCentrality(pin, mode));
		} // end for
		return centrality;	
	} // end method
	
	// [TODO]
	Number getNetCentrality(Rsyn::Net net, const TimingMode mode) const {
		Number centrality = 0;
		for (Rsyn::Pin pin : net.allPins(Rsyn::DRIVER)) {
			centrality = std::max(centrality, getPinCentrality(pin, mode));
		} // end for
		return centrality;	
	} // end method	

	// Get critical arc driving a pin (to) for a specific output transition.
	Rsyn::Arc getCriticalTimingArcToPin(Rsyn::Pin to, const TimingMode mode, const TimingTransition oedge) const {
		Rsyn::Arc worstArcPtr = nullptr;
		Number worstSlack = +std::numeric_limits<Number>::max();
		
		for (Rsyn::Arc arc : to.allIncomingArcs()) {
			const TimingArc &timingArc = getTimingArc(arc);
			
			Rsyn::Pin from = arc.getFromPin();
			const TimingTransition iedge = timingArc.state[mode].backtrack[oedge];
			Number slack = getPinSlack(from, mode, iedge);
			if (slack < worstSlack) {
				worstSlack = slack;
				worstArcPtr = arc;
			} // end if
		} // end for
		
		return worstArcPtr;
	} // end method
	
	// Get critical arc driving a pin (to).
	Rsyn::Arc getCriticalTimingArcToPin(Rsyn::Pin to, const TimingMode mode) const {
		Rsyn::Arc worstArcPtr = nullptr;
		Number worstSlack = +std::numeric_limits<Number>::max();
		
		for (Rsyn::Arc arc : to.allIncomingArcs()) {
			Rsyn::Pin from = arc.getFromPin();
			Number slack = getPinWorstSlack(from, mode);
			if (slack < worstSlack) {
				worstSlack = slack;
				worstArcPtr = arc;
			} // end if
		} // end for
		
		return worstArcPtr;
	} // end method	
	
	// Returns the max delay among all cell arcs.
	Number getCellMaxDelay(Rsyn::Instance cell, const TimingMode mode) const {
		Number maxDelay = 0;
		for (Rsyn::Arc arc : cell.allArcs()) {
			const TimingArc &timingArc = getTimingArc(arc);
			maxDelay = std::max(maxDelay, timingArc.state[mode].delay.getMax());
		} // end for
		return maxDelay;
	} // end method

	// Returns the max load at output pins of this cell.
	Number getCellMaxLoad(Rsyn::Instance cell, const TimingMode mode) const {
		Number maxLoad = 0;
		for (Rsyn::Pin pin : cell.allPins(Rsyn::OUT)) {
			Rsyn::Net net = pin.getNet();
			if (net) {
				maxLoad = std::max(maxLoad, getNetLoad(net).getMax());
			} // end if
		} // end for
		return maxLoad;
	} // end method
	
	// Indicates if timing was computed for this pin. Some pins as unconnected
	// pins do not have timing asserted.
	bool isTimingAsserted(const TimingPin &timingPin, const TimingMode mode, const TimingTransition edge) const { 
		return 
				!isUninitializedValue(timingPin.state[mode].a[edge]) &&
				!isUninitializedValue(timingPin.state[mode].q[edge]); 
	} // end method
	
	bool isTimingAsserted(Rsyn::Pin pin, const TimingMode mode, const TimingTransition edge) const { 
		return isTimingAsserted(getTimingPin(pin), mode, edge); 
	} // end method
		
	const pair<Rsyn::Pin, TimingTransition> &getCriticalPathEndpoint(const TimingMode mode) const {
		return clsCriticalPathEndpoint[mode];}
		
	bool isUninitializedValue(Number value) const { 
		return std::abs(value) == UNINITVALUE; 
	} // end method
	
	EdgeArray<Number> getNetLoad(Rsyn::Net net) const {
		EdgeArray<Number> load(0, 0);
		timingModel->calculateLoadCapacitance(net.getAnyDriver(), LATE, load);
		return load;
	} // end method

	EdgeArray<Number> getNetLoad(Rsyn::Net net, const TimingMode mode) const {
		EdgeArray<Number> load(0, 0);
		timingModel->calculateLoadCapacitance(net.getAnyDriver(), mode, load);
		return load;
	} // end method
	
	// [TODO] Improve this.
	Rsyn::Pin getClockPin(Rsyn::Instance cell) const {
		Rsyn::Pin clock = nullptr;
		for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) {
			const TimingPin &timingPin = getTimingPin(pin);
			if (timingPin.isClockPin()) {
				clock = pin;
				break;
			} // end if
		} // end for
		
		return clock;
	} // end method
	
	// [TODO] Improve this.
	Rsyn::Pin getDataPin(Rsyn::Instance cell) const {
		Rsyn::Pin data = nullptr;
		for (Rsyn::Pin pin : cell.allPins(Rsyn::IN)) {
			const TimingPin &timingPin = getTimingPin(pin);
			if (timingPin.isDataPin()) {
				data = pin;
				break;
			} // end if
		} // end for
		
		return data;
	} // end method
	
	EdgeArray<Number> getSetupTime(Rsyn::Instance sequential) const {
		return timingModel->getSetupTime(getDataPin(sequential));
	} // end method

	EdgeArray<Number> getHoldTime(Rsyn::Instance sequential) const {
		return timingModel->getHoldTime(getDataPin(sequential));
	} // end method
	
	Number getClockPinLatency(Rsyn::Instance sequential, const TimingMode mode, const TimingTransition edge) const {
		Rsyn::Pin clock = getClockPin(sequential);
		return clock? getPinArrivalTime(clock, mode, edge) : 0;
	} // end method
	
	Number getClockToQDelay(Rsyn::Instance sequential, const TimingMode mode) const {
		Number delay = 0;
		Rsyn::Pin clock = getClockPin(sequential);
		if (clock) {
			for (Rsyn::Arc arc : clock.allOutgoingArcs()) {
				const TimingArc &timingArc = getTimingArc(arc);
				delay = std::max(delay, timingArc.state[mode].delay.getMax());
			} // end for
		} // end if
		return delay;
	} // end method

	////////////////////////////////////////////////////////////////////////////
	// Runtime
	////////////////////////////////////////////////////////////////////////////
	
private:
	
	// Runtime breakdown
	Stopwatch clsStopwatchUpdateTiming;
	
public:
	
	void resetRuntime() { clsStopwatchUpdateTiming.reset(); }
	const Stopwatch &getUpdateTimingRuntime() const { return clsStopwatchUpdateTiming; }	
	
	////////////////////////////////////////////////////////////////////////////
	// Top Critical Paths
	////////////////////////////////////////////////////////////////////////////

public:  
  
	class PathHop {
	friend class Timer;
	private:
		Rsyn::Arc rsynArcFromThisPin;
		Rsyn::Arc rsynArcToThisPin;
		Number arrival;
		Number required;
		Number delay;
		Rsyn::Pin pin;
		Rsyn::Pin previousPin;
		Rsyn::Pin nextPin;
		TimingTransition transition;
		TimingMode mode; // stored for commodity, used to compute slack
		
		PathHop() :
		arrival(std::numeric_limits<Number>::quiet_NaN()),
		required(std::numeric_limits<Number>::quiet_NaN()),
		delay(std::numeric_limits<Number>::quiet_NaN()),
		pin(nullptr),
		previousPin(nullptr),
		nextPin(nullptr),
		rsynArcFromThisPin(nullptr),
		rsynArcToThisPin(nullptr){}
				
	public:
		
		Rsyn::Pin getPin() const { return pin; }
		Rsyn::Pin getPreviousPin() const { return previousPin; }
		Rsyn::Pin getNextPin() const { return nextPin; }
				
		Rsyn::Net getNet() const { return pin.getNet(); }
		Rsyn::Instance getInstance() const { return pin.getInstance(); }
		
		TimingMode getTimingMode() const { return mode; }
		TimingTransition getTransition() const { return transition; }
		Number getArrival() const { return arrival; }
		Number getRequired() const { return required; }
		
		// Computes the slack at this pin in this path.
		// Remember this is not necessarily the worst slack seen at this pin.
		// Note that the slack should be the same at all pin in this path.
		Number getSlack() const {
			switch (mode) {
				case EARLY: return arrival - required;
				case LATE : return required - arrival;
				default: assert(false);
			} // end switch
		} // end method
		
		// Get the delay from the previous pin to the current pin. For 
		// startpoint this is the input delay.
		Number getDelay() const { return delay; }
		
		// Returns the arc, if any, from this pin.
		Rsyn::Arc getArcFromThisPin() const { return rsynArcFromThisPin; }
		
		// Returns the arc, if any, to this pin.
		Rsyn::Arc getArcToThisPin() const { return rsynArcToThisPin; }
	}; // end class	
	
private:

	// Helper structure used to generate critical paths.
	struct Reference {
		Rsyn::Pin propPin;
		TimingArc * propArcPointerFromThisPin;
		Rsyn::Arc propRsynArcFromThisPin;
		TimingTransition propTransition;
		Number propRequired;
		int propParentPartialPath;
		TimingTransition propTransitionAtParent;
		
		// This is not the actual slack in the current path. This is computed
		// using the path required (maybe different from the worst required seen
		// at this pin) and the worst arrival time.
		// The actual slack in this path will be computed later during the 
		// backtracking.
		Number propSlack; 
		
		// Constructor.
		Reference() :
			propPin(nullptr),
			propArcPointerFromThisPin(nullptr),
			propRsynArcFromThisPin(nullptr),
			propRequired(std::numeric_limits<Number>::quiet_NaN()),
			propSlack(std::numeric_limits<Number>::quiet_NaN()),
			propParentPartialPath(-1)
		{}
		
		Reference(Rsyn::Pin pin, 
				TimingArc * arc,
				Rsyn::Arc rsynArc,
				const Number required,
				const Number slack, 
				const TimingTransition transition,
				const int parent,
				const TimingTransition transitionAtParent) :
				
				propPin(pin),
				propArcPointerFromThisPin(arc),
				propRsynArcFromThisPin(rsynArc),
				propRequired(required),
				propSlack(slack),
				propTransition(transition),
				propParentPartialPath(parent),
				propTransitionAtParent(transitionAtParent) {}
					
		bool operator>(const Reference &rhs) const {
			return (propSlack > rhs.propSlack);
		} // end if
		
		void print(const std::string &caption, ostream &out, Rsyn::Design design) const {
			out << caption << ": "
				<< propPin.getFullName() << " "
				<< "q=" << propRequired << " "
				<< "slack=" << propSlack << " "
				<< "transition=" << (propTransition==RISE?"rise" : "fall") << " "
				<< "parent=" << propParentPartialPath << " "
				<< "parent_transition=" << propTransitionAtParent << "\n";			
		} // end operator
	}; // end struct    
	
	typedef std::priority_queue<
		Reference, std::deque<Reference>, std::greater<Reference>> CriticalPathQueue;
	
	void queryTopCriticalPaths_Queue_AddCriticalEndpoint(CriticalPathQueue &queue,
			Rsyn::Pin endpoint,
			const TimingMode mode,
			const Number slackThreshold,
			const bool checkSign,
			const int sign,			
			const bool debug);

	void queryTopCriticalPaths_Queue_AddAllCriticalEndpoints(CriticalPathQueue &queue,
			const TimingMode mode,
			const Number slackThreshold,
			const bool checkSign,
			const int sign,
			const bool debug);
	
	bool queryTopCriticalPaths_Internal(CriticalPathQueue &queue,
			const TimingMode mode, 
			const int maxNumPaths,
			std::vector<std::vector<PathHop>> &paths, 
			const Number slackThreshold,
			const bool checkSign,
			const int sign,
			const bool debug);

public:

	// Returns the top most critical paths with slack less than slackThreshold.
	// Paths are sorted by increasing slack (most negative first), that is, the
	// most critical path is stored in the index zero.
	// The first hop in the path is the path startpoint and the last one is the
	// endpoint.
	
	bool queryTopCriticalPaths(
			const TimingMode mode, 
			const int maxNumPaths,
			std::vector<std::vector<PathHop>> &paths, 
			const Number slackThreshold = 0
	);	
	
	// Same as the method above, but only return the most critical path if any.
	bool queryTopCriticalPath(
			const TimingMode mode,
			std::vector<PathHop> &path, 
			const Number slackThreshold = 0
	) {
		path.clear();
		std::vector<std::vector<PathHop>> paths;
		const bool success = queryTopCriticalPaths(mode, 1, paths, slackThreshold);
		if (success)
			path.swap(paths[0]);
		return success;
	} // end method

	// Returns one critical path per endpoint. The path from the most critical
	// endpoint is at index 0. Returns the number of paths found.
	int queryTopCriticalPathFromTopCriticalEndpoints(
			const TimingMode mode, 
			const int maxNumEndpoints,
			std::vector<std::vector<PathHop>> &paths, 
			const Number slackThreshold = 0);
	
	// Returns the top critical paths associated to the top critical endpoints. 
	// The bi-dimensional path vector returned is sorted by decreasing endpoint
	// criticality and then by decreasing path criticality. That is, the top
	// most critical path is at index [0][0] and the top 3 critical path of
	// top 5 endpoint is at index [4][2]. Returns the number of paths found.
	int queryTopCriticalPathsFromTopCriticalEndpoints(
		const TimingMode mode, 
		const int maxNumEndpoints,
		const int maxNumPathsPerEndpoint,
		std::vector<std::vector<std::vector<PathHop>>> &paths, 
		const Number slackThreshold	= 0
	);
	
	// Same as above, but the paths are returned on an one-dimensional vector.
	int  queryTopCriticalPathsFromTopCriticalEndpointsConsolidated(
		const TimingMode mode, 
		const int maxNumEndpoints,
		const int maxNumPathsPerEndpoint,		
		std::vector<std::vector<PathHop>> &paths, 
		const Number slackThreshold	= 0
	) {
		std::vector<std::vector<std::vector<PathHop>>> multiPaths;
		const int numPaths = queryTopCriticalPathsFromTopCriticalEndpoints(mode, 
				maxNumEndpoints, maxNumPathsPerEndpoint, multiPaths, slackThreshold);
		
		paths.clear();
		paths.reserve(numPaths); // defensive programming

		int pathCounter = 0;
		const int actualNumEndpoints = multiPaths.size();
		for (int i = 0; i < actualNumEndpoints; i++) {
			const int actualNumPaths = multiPaths[i].size();
			for (int k = 0; k < actualNumPaths; k++) {		
				paths.resize(paths.size() + 1);
				paths.back().swap(multiPaths[i][k]);
				pathCounter++;
			} // end for
		} // end for
		
		if (pathCounter != numPaths) {
			std::cout << "[BUG] Number of paths reported is different from the actual number of paths.\n";
		} // end if
		
		return pathCounter;
	} // end method
  
	// TODO
	int queryTopCriticalPathsFromEndpoint( 
			const TimingMode mode, 
			const Rsyn::Pin endpoint,
			const int maxNumPaths,		
			std::vector<std::vector<PathHop>> &paths, 
			const Number slackThreshold	= 0);

	// Returns the top critical paths passing thru a particular pin.
	int queryTopCriticalPathsPassingThruPin( 
			const TimingMode mode, 
			const Rsyn::Pin referencePin,
			const int maxNumPaths,		
			std::vector<std::vector<PathHop>> &paths, 
			const Number slackThreshold	= 0);
	
	// Update timing information of the path to match the current timing state.
	void updatePath(std::vector<PathHop> &path);
	
	// Report paths.
	void reportPath(const std::vector<PathHop> &path, std::ostream &out = std::cout);
	void reportPaths(const std::vector<std::vector<PathHop>> &paths, std::ostream &out = std::cout);
	
	void reportCriticalPath(const TimingMode mode, std::ostream &out = std::cout);
	
	// Report endpoints.
	void reportEndpoints(const TimingMode mode, const int top, const Number slackThreshold = 0, std::ostream &out = std::cout);
	
	// Get the current hop delay.
	// Note that this is based on the current timing information, which may be
	// different from the timing information when the path was generated.
	// If the hop is an output pin, the returned delay is the arc delay to the
	// pin represented by the hop. If the hop is an input pin, the returned
	// delay is the wire delay.
	Number getPathHopDelay(const PathHop &hop) {
		if (hop.getArcToThisPin()) {
			const TimingArc &timingArc = getTimingArc(hop.getArcToThisPin());
			return timingArc.state[hop.getTimingMode()].delay[hop.getTransition()];
		} else {
			if (!hop.getPreviousPin()) {
				// Starting point.
				const TimingPin &timingPin = getTimingPin(hop.getPin());
				return timingPin.state[hop.getTimingMode()].a[hop.getTransition()];
			} else {
				const TimingPin &timingPin = getTimingPin(hop.getPin());
				return timingPin.state[hop.getTimingMode()].wdelay[hop.getTransition()];
			} // end else
		} // end else		
	} // end method

	// Split the delay of a path into cell and wire components.
	void getPathDelayDueToCellsAndWires(const std::vector<PathHop> &path,
			Number &delayDueToCell,
			Number &delayDueToWire);	
	
	// Returns a hash that identifies this path.
	std::string getPathHash(const std::vector<PathHop> &path, const bool ignoreTransitions) const;

	////////////////////////////////////////////////////////////////////////////
	// Endpoints
	////////////////////////////////////////////////////////////////////////////
	
	bool queryTopCriticalEndpoints(
			const TimingMode mode, 
			const int maxNumEndpoints,
			std::vector<Rsyn::Pin> &endpoints, 
			const Number slackThreshold = 0);	
	
	////////////////////////////////////////////////////////////////////////////
	// Histogram 
	////////////////////////////////////////////////////////////////////////////
	
	int countNumberNegativeSlackCells(const TimingMode mode);	

	void slackHistrogram(ostream &out, TimingMode mode );
	
	////////////////////////////////////////////////////////////////////////////
	// Debug Reports
	////////////////////////////////////////////////////////////////////////////

	void dumpTiming(const std::string &filename);
	void printTimingPropagation(ostream &out, bool newLine = false );
	void printPinDebug(ostream &out, Rsyn::Pin pin, const bool early = true, const bool late = true);
	void printArcDebug(ostream &out, Rsyn::Arc arc);
	void printNetDebug(ostream &out, Rsyn::Net net);
	void printState(ostream &out, const std::array<TimingPinState, NUM_TIMING_MODES> &state);
	
	void writeTimingFile(ostream &out);

	////////////////////////////////////////////////////////////////////////////
	// For eaches
	////////////////////////////////////////////////////////////////////////////
			
	std::array<TimingMode, 2>
	allTimingModes() const {
		return {EARLY, LATE};
	} // end method
	
	// -----------------------------------------------------------------------------

	std::array<TimingTransition, 2> 
	allTimingTransitions() const {
		return {FALL, RISE};
	} // end method
	
	// -----------------------------------------------------------------------------

	std::array<std::tuple<TimingTransition, TimingTransition>, 4>
	allTimingTransitionPairs() const {
		return {
			std::make_tuple(FALL, FALL), 
			std::make_tuple(FALL, RISE),
			std::make_tuple(RISE, FALL),
			std::make_tuple(RISE, RISE)};
	} // end method
		
	// -----------------------------------------------------------------------------
	
	std::array<std::tuple<TimingMode, TimingTransition>, 4>
	allTimingModeAndTransitionPairs() const {
		return {
			std::make_tuple(EARLY, FALL), 
			std::make_tuple(EARLY, RISE),
			std::make_tuple(LATE, FALL),
			std::make_tuple(LATE, RISE)};
	} // end method

	// -------------------------------------------------------------------------
	
	const std::set<Rsyn::Cell> &allSequentialCells() const {
		return sequentialCells;
	} // end method
		
	// -------------------------------------------------------------------------
	
	const std::set<Rsyn::Pin> &allEndpoints() const {
		return endpoints;
	} // end method
		
	// -------------------------------------------------------------------------
	
	const std::set<Rsyn::Pin> &allTies() const {
		return ties;
	} // end method
	
}; // end class

} // end namespace

#endif
