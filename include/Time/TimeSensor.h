/*
 * Copyright 2009 Nathan Matthews <lowentropy@gmail.com>
 *
 * This file is part of SimpleX3D.
 * 
 * SimpleX3D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SimpleX3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SimpleX3D.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _X3D_TIMESENSOR_H_
#define _X3D_TIMESENSOR_H_

#include "Core/X3DSensorNode.h"
#include "Time/X3DTimeDependentNode.h"

using X3D::Core::X3DSensorNode;

namespace X3D {
namespace Time {

/**
 * These nodes generate events as time passes. They can be
 * configured as one-shot or repeating event triggers, or as
 * continuous animation drivers.
 */
class TimeSensor: public X3DTimeDependentNode, public X3DSensorNode {
public:

    TimeSensor() : next(-1) {}

	/// Period of time for repeated events.
	class CycleInterval : public DefaultInOutField<TimeSensor, SFTime> {
		bool filter(double interval) {
            if (interval == value())
                return false;
            if (node()->getIsActive())
                return false;
            return true;
		}
	} cycleInterval;

	/// Sends current time on startTime or cycle loop
	DefaultOutField<TimeSensor, SFTime> cycleTime;

	/// Fraction of current cycle.
	DefaultOutField<TimeSensor, SFFloat> fraction_changed;

	/// Current timestamp.
	DefaultOutField<TimeSensor, SFTime> time;

    /// Setup function called on instantiation.
    virtual void setup();

    /// Initialize the timesensor.
    void initSensor();

    /// Evaluate the sensor
    void evaluate();

    /// Schedule waking at given time, unless already scheduled to wake up before this.
    void wake(double time);

    /// Tick the continuous events
    bool tick();

    /// We actually have a field for this now.
    bool getIsActive() const;

private:

    /// elapsed (not counting pause) since cycle event
    double elapsed;

    /// last tick time
    double last;

    /// next scheduled evaluation time
    double next;

    /// network sort of four elements
    void sortEvents(double* times, int* indexes);

    /// no copy constructor
    TimeSensor(const TimeSensor& node) { throw X3DError("COPY CONSTRUCTOR"); }
};

}}

#endif // #ifndef _X3D_TIMESENSOR_H_
