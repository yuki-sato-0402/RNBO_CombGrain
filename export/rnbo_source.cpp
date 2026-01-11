/*******************************************************************************************************************
Copyright (c) 2023 Cycling '74

The code that Max generates automatically and that end users are capable of
exporting and using, and any associated documentation files (the “Software”)
is a work of authorship for which Cycling '74 is the author and owner for
copyright purposes.

This Software is dual-licensed either under the terms of the Cycling '74
License for Max-Generated Code for Export, or alternatively under the terms
of the General Public License (GPL) Version 3. You may use the Software
according to either of these licenses as it is most appropriate for your
project on a case-by-case basis (proprietary or not).

A) Cycling '74 License for Max-Generated Code for Export

A license is hereby granted, free of charge, to any person obtaining a copy
of the Software (“Licensee”) to use, copy, modify, merge, publish, and
distribute copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The Software is licensed to Licensee for all uses that do not include the sale,
sublicensing, or commercial distribution of software that incorporates this
source code. This means that the Licensee is free to use this software for
educational, research, and prototyping purposes, to create musical or other
creative works with software that incorporates this source code, or any other
use that does not constitute selling software that makes use of this source
code. Commercial distribution also includes the packaging of free software with
other paid software, hardware, or software-provided commercial services.

For entities with UNDER $200k in annual revenue or funding, a license is hereby
granted, free of charge, for the sale, sublicensing, or commercial distribution
of software that incorporates this source code, for as long as the entity's
annual revenue remains below $200k annual revenue or funding.

For entities with OVER $200k in annual revenue or funding interested in the
sale, sublicensing, or commercial distribution of software that incorporates
this source code, please send inquiries to licensing@cycling74.com.

The above copyright notice and this license shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

Please see
https://support.cycling74.com/hc/en-us/articles/10730637742483-RNBO-Export-Licensing-FAQ
for additional information

B) General Public License Version 3 (GPLv3)
Details of the GPLv3 license can be found at: https://www.gnu.org/licenses/gpl-3.0.html
*******************************************************************************************************************/

#ifdef RNBO_LIB_PREFIX
#define STR_IMPL(A) #A
#define STR(A) STR_IMPL(A)
#define RNBO_LIB_INCLUDE(X) STR(RNBO_LIB_PREFIX/X)
#else
#define RNBO_LIB_INCLUDE(X) #X
#endif // RNBO_LIB_PREFIX
#ifdef RNBO_INJECTPLATFORM
#define RNBO_USECUSTOMPLATFORM
#include RNBO_INJECTPLATFORM
#endif // RNBO_INJECTPLATFORM

#include RNBO_LIB_INCLUDE(RNBO_Common.h)
#include RNBO_LIB_INCLUDE(RNBO_AudioSignal.h)

namespace RNBO {


#define trunc(x) ((Int)(x))
#define autoref auto&

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

template <class ENGINE = INTERNALENGINE> class rnbomatic : public PatcherInterfaceImpl {

friend class EngineCore;
friend class Engine;
friend class MinimalEngine<>;
public:

rnbomatic()
: _internalEngine(this)
{
}

~rnbomatic()
{
    deallocateSignals();
}

Index getNumMidiInputPorts() const {
    return 1;
}

void processMidiEvent(MillisecondTime time, int port, ConstByteArray data, Index length) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->ctlin_01_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_02_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_03_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_04_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_05_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
    this->ctlin_06_midihandler(data[0] & 240, (data[0] & 15) + 1, port, data, length);
}

Index getNumMidiOutputPorts() const {
    return 0;
}

void process(
    const SampleValue * const* inputs,
    Index numInputs,
    SampleValue * const* outputs,
    Index numOutputs,
    Index n
) {
    this->vs = n;
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr, true);
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);

    this->gen_01_perform(
        in1,
        in2,
        this->gen_01_movDur,
        this->gen_01_grainSize,
        this->gen_01_reverse,
        this->gen_01_Mix,
        this->gen_01_delTime,
        this->gen_01_feedCoe,
        this->gen_01_density,
        out1,
        out2,
        n
    );

    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->advanceTime((ENGINE*)nullptr);
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
    RNBO_ASSERT(this->_isInitialized);

    if (this->maxvs < maxBlockSize || !this->didAllocateSignals) {
        this->globaltransport_tempo = resizeSignal(this->globaltransport_tempo, this->maxvs, maxBlockSize);
        this->globaltransport_state = resizeSignal(this->globaltransport_state, this->maxvs, maxBlockSize);
        this->zeroBuffer = resizeSignal(this->zeroBuffer, this->maxvs, maxBlockSize);
        this->dummyBuffer = resizeSignal(this->dummyBuffer, this->maxvs, maxBlockSize);
        this->didAllocateSignals = true;
    }

    const bool sampleRateChanged = sampleRate != this->sr;
    const bool maxvsChanged = maxBlockSize != this->maxvs;
    const bool forceDSPSetup = sampleRateChanged || maxvsChanged || force;

    if (sampleRateChanged || maxvsChanged) {
        this->vs = maxBlockSize;
        this->maxvs = maxBlockSize;
        this->sr = sampleRate;
        this->invsr = 1 / sampleRate;
    }

    this->data_01_dspsetup(forceDSPSetup);
    this->data_02_dspsetup(forceDSPSetup);
    this->gen_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
}

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->ringBuffIndex);
        break;
        }
    case 1:
        {
        return addressOf(this->ringBuff);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_01_delL_bufferobj);
        break;
        }
    case 3:
        {
        return addressOf(this->gen_01_delR_bufferobj);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 4;
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time, (ENGINE*)nullptr);

    if (index == 0) {
        this->data_01_buffer = reInitDataView(this->data_01_buffer, this->ringBuffIndex);
        this->data_01_bufferUpdated();
        this->gen_01_RingBuffIndex = reInitDataView(this->gen_01_RingBuffIndex, this->ringBuffIndex);
    }

    if (index == 1) {
        this->data_02_buffer = reInitDataView(this->data_02_buffer, this->ringBuff);
        this->data_02_bufferUpdated();
        this->gen_01_RingBuff = reInitDataView(this->gen_01_RingBuff, this->ringBuff);
    }

    if (index == 2) {
        this->gen_01_delL_buffer = reInitDataView(this->gen_01_delL_buffer, this->gen_01_delL_bufferobj);
    }

    if (index == 3) {
        this->gen_01_delR_buffer = reInitDataView(this->gen_01_delR_buffer, this->gen_01_delR_bufferobj);
    }
}

void initialize() {
    RNBO_ASSERT(!this->_isInitialized);

    this->ringBuffIndex = initDataRef(
        this->ringBuffIndex,
        this->dataRefStrings->name0,
        false,
        this->dataRefStrings->file0,
        this->dataRefStrings->tag0
    );

    this->ringBuff = initDataRef(
        this->ringBuff,
        this->dataRefStrings->name1,
        false,
        this->dataRefStrings->file1,
        this->dataRefStrings->tag1
    );

    this->gen_01_delL_bufferobj = initDataRef(
        this->gen_01_delL_bufferobj,
        this->dataRefStrings->name2,
        true,
        this->dataRefStrings->file2,
        this->dataRefStrings->tag2
    );

    this->gen_01_delR_bufferobj = initDataRef(
        this->gen_01_delR_bufferobj,
        this->dataRefStrings->name3,
        true,
        this->dataRefStrings->file3,
        this->dataRefStrings->tag3
    );

    this->assign_defaults();
    this->applyState();
    this->ringBuffIndex->setIndex(0);
    this->data_01_buffer = new Float32Buffer(this->ringBuffIndex);
    this->gen_01_RingBuffIndex = new Float32Buffer(this->ringBuffIndex);
    this->ringBuff->setIndex(1);
    this->data_02_buffer = new Float32Buffer(this->ringBuff);
    this->gen_01_RingBuff = new Float32Buffer(this->ringBuff);
    this->gen_01_delL_bufferobj->setIndex(2);
    this->gen_01_delL_buffer = new Float64Buffer(this->gen_01_delL_bufferobj);
    this->gen_01_delR_bufferobj->setIndex(3);
    this->gen_01_delR_buffer = new Float64Buffer(this->gen_01_delR_bufferobj);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
    this->_isInitialized = true;
}

void getPreset(PatcherStateInterface& preset) {
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr);
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "density"));
    this->param_02_getPresetValue(getSubState(preset, "Mix"));
    this->param_03_getPresetValue(getSubState(preset, "movDur"));
    this->param_04_getPresetValue(getSubState(preset, "grainSize"));
    this->param_05_getPresetValue(getSubState(preset, "feedCoe"));
    this->param_06_getPresetValue(getSubState(preset, "delTime"));
    this->param_07_getPresetValue(getSubState(preset, "reverse"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->param_01_setPresetValue(getSubState(preset, "density"));
    this->param_02_setPresetValue(getSubState(preset, "Mix"));
    this->param_03_setPresetValue(getSubState(preset, "movDur"));
    this->param_04_setPresetValue(getSubState(preset, "grainSize"));
    this->param_05_setPresetValue(getSubState(preset, "feedCoe"));
    this->param_06_setPresetValue(getSubState(preset, "delTime"));
    this->param_07_setPresetValue(getSubState(preset, "reverse"));
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time, (ENGINE*)nullptr);

    switch (index) {
    case 0:
        {
        this->param_01_value_set(v);
        break;
        }
    case 1:
        {
        this->param_02_value_set(v);
        break;
        }
    case 2:
        {
        this->param_03_value_set(v);
        break;
        }
    case 3:
        {
        this->param_04_value_set(v);
        break;
        }
    case 4:
        {
        this->param_05_value_set(v);
        break;
        }
    case 5:
        {
        this->param_06_value_set(v);
        break;
        }
    case 6:
        {
        this->param_07_value_set(v);
        break;
        }
    }
}

void processParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValue(index, value, time);
}

void processParameterBangEvent(ParameterIndex index, MillisecondTime time) {
    this->setParameterValue(index, this->getParameterValue(index), time);
}

void processNormalizedParameterEvent(ParameterIndex index, ParameterValue value, MillisecondTime time) {
    this->setParameterValueNormalized(index, value, time);
}

ParameterValue getParameterValue(ParameterIndex index)  {
    switch (index) {
    case 0:
        {
        return this->param_01_value;
        }
    case 1:
        {
        return this->param_02_value;
        }
    case 2:
        {
        return this->param_03_value;
        }
    case 3:
        {
        return this->param_04_value;
        }
    case 4:
        {
        return this->param_05_value;
        }
    case 5:
        {
        return this->param_06_value;
        }
    case 6:
        {
        return this->param_07_value;
        }
    default:
        {
        return 0;
        }
    }
}

ParameterIndex getNumSignalInParameters() const {
    return 0;
}

ParameterIndex getNumSignalOutParameters() const {
    return 0;
}

ParameterIndex getNumParameters() const {
    return 7;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "density";
        }
    case 1:
        {
        return "Mix";
        }
    case 2:
        {
        return "movDur";
        }
    case 3:
        {
        return "grainSize";
        }
    case 4:
        {
        return "feedCoe";
        }
    case 5:
        {
        return "delTime";
        }
    case 6:
        {
        return "reverse";
        }
    default:
        {
        return "bogus";
        }
    }
}

ConstCharPointer getParameterId(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "density";
        }
    case 1:
        {
        return "Mix";
        }
    case 2:
        {
        return "movDur";
        }
    case 3:
        {
        return "grainSize";
        }
    case 4:
        {
        return "feedCoe";
        }
    case 5:
        {
        return "delTime";
        }
    case 6:
        {
        return "reverse";
        }
    default:
        {
        return "bogus";
        }
    }
}

void getParameterInfo(ParameterIndex index, ParameterInfo * info) const {
    {
        switch (index) {
        case 0:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 50;
            info->min = 1;
            info->max = 100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 1:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 50;
            info->min = 0;
            info->max = 100;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 500;
            info->min = 5;
            info->max = 1000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 3:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1000;
            info->min = 10;
            info->max = 2000;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0.5;
            info->min = 0.01;
            info->max = 0.9;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 5:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 100;
            info->min = 2;
            info->max = 2500;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        case 6:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 0;
            info->min = 0;
            info->max = 1;
            info->exponent = 1;
            info->steps = 0;
            info->debug = false;
            info->saveable = true;
            info->transmittable = true;
            info->initialized = true;
            info->visible = true;
            info->displayName = "";
            info->unit = "";
            info->ioType = IOTypeUndefined;
            info->signalIndex = INVALID_INDEX;
            break;
            }
        }
    }
}

ParameterValue applyStepsToNormalizedParameterValue(ParameterValue normalizedValue, int steps) const {
    if (steps == 1) {
        if (normalizedValue > 0) {
            normalizedValue = 1.;
        }
    } else {
        ParameterValue oneStep = (number)1. / (steps - 1);
        ParameterValue numberOfSteps = rnbo_fround(normalizedValue / oneStep * 1 / (number)1) * (number)1;
        normalizedValue = numberOfSteps * oneStep;
    }

    return normalizedValue;
}

ParameterValue convertToNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 6:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 100 ? 100 : value));
            ParameterValue normalizedValue = (value - 0) / (100 - 0);
            return normalizedValue;
        }
        }
    case 0:
        {
        {
            value = (value < 1 ? 1 : (value > 100 ? 100 : value));
            ParameterValue normalizedValue = (value - 1) / (100 - 1);
            return normalizedValue;
        }
        }
    case 5:
        {
        {
            value = (value < 2 ? 2 : (value > 2500 ? 2500 : value));
            ParameterValue normalizedValue = (value - 2) / (2500 - 2);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 5 ? 5 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 5) / (1000 - 5);
            return normalizedValue;
        }
        }
    case 3:
        {
        {
            value = (value < 10 ? 10 : (value > 2000 ? 2000 : value));
            ParameterValue normalizedValue = (value - 10) / (2000 - 10);
            return normalizedValue;
        }
        }
    case 4:
        {
        {
            value = (value < 0.01 ? 0.01 : (value > 0.9 ? 0.9 : value));
            ParameterValue normalizedValue = (value - 0.01) / (0.9 - 0.01);
            return normalizedValue;
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue convertFromNormalizedParameterValue(ParameterIndex index, ParameterValue value) const {
    value = (value < 0 ? 0 : (value > 1 ? 1 : value));

    switch (index) {
    case 6:
        {
        {
            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 1:
        {
        {
            {
                return 0 + value * (100 - 0);
            }
        }
        }
    case 0:
        {
        {
            {
                return 1 + value * (100 - 1);
            }
        }
        }
    case 5:
        {
        {
            {
                return 2 + value * (2500 - 2);
            }
        }
        }
    case 2:
        {
        {
            {
                return 5 + value * (1000 - 5);
            }
        }
        }
    case 3:
        {
        {
            {
                return 10 + value * (2000 - 10);
            }
        }
        }
    case 4:
        {
        {
            {
                return 0.01 + value * (0.9 - 0.01);
            }
        }
        }
    default:
        {
        return value;
        }
    }
}

ParameterValue constrainParameterValue(ParameterIndex index, ParameterValue value) const {
    switch (index) {
    case 0:
        {
        return this->param_01_value_constrain(value);
        }
    case 1:
        {
        return this->param_02_value_constrain(value);
        }
    case 2:
        {
        return this->param_03_value_constrain(value);
        }
    case 3:
        {
        return this->param_04_value_constrain(value);
        }
    case 4:
        {
        return this->param_05_value_constrain(value);
        }
    case 5:
        {
        return this->param_06_value_constrain(value);
        }
    case 6:
        {
        return this->param_07_value_constrain(value);
        }
    default:
        {
        return value;
        }
    }
}

void processNumMessage(MessageTag , MessageTag , MillisecondTime , number ) {}

void processListMessage(MessageTag , MessageTag , MillisecondTime , const list& ) {}

void processBangMessage(MessageTag , MessageTag , MillisecondTime ) {}

MessageTagInfo resolveTag(MessageTag tag) const {
    switch (tag) {

    }

    return "";
}

MessageIndex getNumMessages() const {
    return 0;
}

const MessageInfo& getMessageInfo(MessageIndex index) const {
    switch (index) {

    }

    return NullMessageInfo;
}

protected:

		
void advanceTime(EXTERNALENGINE*) {}
void advanceTime(INTERNALENGINE*) {
	_internalEngine.advanceTime(sampstoms(this->vs));
}

void processInternalEvents(MillisecondTime time) {
	_internalEngine.processEventsUntil(time);
}

void updateTime(MillisecondTime time, INTERNALENGINE*, bool inProcess = false) {
	if (time == TimeNow) time = getPatcherTime();
	processInternalEvents(inProcess ? time + sampsToMs(this->vs) : time);
	updateTime(time, (EXTERNALENGINE*)nullptr);
}

rnbomatic* operator->() {
    return this;
}
const rnbomatic* operator->() const {
    return this;
}
rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template<typename LISTTYPE = list> void listquicksort(LISTTYPE& arr, LISTTYPE& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template<typename LISTTYPE = list> Int listpartition(LISTTYPE& arr, LISTTYPE& sortindices, Int l, Int h, bool ascending) {
    number x = arr[(Index)h];
    Int i = (Int)(l - 1);

    for (Int j = (Int)(l); j <= h - 1; j++) {
        bool asc = (bool)((bool)(ascending) && arr[(Index)j] <= x);
        bool desc = (bool)((bool)(!(bool)(ascending)) && arr[(Index)j] >= x);

        if ((bool)(asc) || (bool)(desc)) {
            i++;
            this->listswapelements(arr, i, j);
            this->listswapelements(sortindices, i, j);
        }
    }

    i++;
    this->listswapelements(arr, i, h);
    this->listswapelements(sortindices, i, h);
    return i;
}

template<typename LISTTYPE = list> void listswapelements(LISTTYPE& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
    number a1 = 1. + a;
    number aa = a * a1;
    number b = 1. - a;
    number b1 = 2. - a;
    number bb = b * b1;
    number fw = -.1666667 * bb * a;
    number fx = .5 * bb * a1;
    number fy = .5 * aa * b1;
    number fz = -.1666667 * aa * b;
    return w * fw + x * fx + y * fy + z * fz;
}

inline number fastcubicinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = z - y - w + x;
    number f1 = w - x - f0;
    number f2 = y - w;
    number f3 = x;
    return f0 * a * a2 + f1 * a2 + f2 * a + f3;
}

inline number splineinterp(number a, number w, number x, number y, number z) {
    number a2 = a * a;
    number f0 = -0.5 * w + 1.5 * x - 1.5 * y + 0.5 * z;
    number f1 = w - 2.5 * x + 2 * y - 0.5 * z;
    number f2 = -0.5 * w + 0.5 * y;
    return f0 * a * a2 + f1 * a2 + f2 * a + x;
}

inline number spline6interp(number a, number y0, number y1, number y2, number y3, number y4, number y5) {
    number ym2py2 = y0 + y4;
    number ym1py1 = y1 + y3;
    number y2mym2 = y4 - y0;
    number y1mym1 = y3 - y1;
    number sixthym1py1 = (number)1 / (number)6.0 * ym1py1;
    number c0 = (number)1 / (number)120.0 * ym2py2 + (number)13 / (number)60.0 * ym1py1 + (number)11 / (number)20.0 * y2;
    number c1 = (number)1 / (number)24.0 * y2mym2 + (number)5 / (number)12.0 * y1mym1;
    number c2 = (number)1 / (number)12.0 * ym2py2 + sixthym1py1 - (number)1 / (number)2.0 * y2;
    number c3 = (number)1 / (number)12.0 * y2mym2 - (number)1 / (number)6.0 * y1mym1;
    number c4 = (number)1 / (number)24.0 * ym2py2 - sixthym1py1 + (number)1 / (number)4.0 * y2;
    number c5 = (number)1 / (number)120.0 * (y5 - y0) + (number)1 / (number)24.0 * (y1 - y4) + (number)1 / (number)12.0 * (y3 - y2);
    return ((((c5 * a + c4) * a + c3) * a + c2) * a + c1) * a + c0;
}

inline number cosT8(number r) {
    number t84 = 56.0;
    number t83 = 1680.0;
    number t82 = 20160.0;
    number t81 = 2.4801587302e-05;
    number t73 = 42.0;
    number t72 = 840.0;
    number t71 = 1.9841269841e-04;

    if (r < 0.785398163397448309615660845819875721 && r > -0.785398163397448309615660845819875721) {
        number rr = r * r;
        return 1.0 - rr * t81 * (t82 - rr * (t83 - rr * (t84 - rr)));
    } else if (r > 0.0) {
        r -= 1.57079632679489661923132169163975144;
        number rr = r * r;
        return -r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    } else {
        r += 1.57079632679489661923132169163975144;
        number rr = r * r;
        return r * (1.0 - t71 * rr * (t72 - rr * (t73 - rr)));
    }
}

inline number cosineinterp(number frac, number x, number y) {
    number a2 = (1.0 - this->cosT8(frac * 3.14159265358979323846)) / (number)2.0;
    return x * (1.0 - a2) + y * a2;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

Index voice() {
    return this->_voiceIndex;
}

number random(number low, number high) {
    number range = high - low;
    return globalrandom() * range + low;
}

template<typename BUFFERTYPE> number dim(BUFFERTYPE& buffer) {
    return buffer->getSize();
}

inline number safemod(number f, number m) {
    if (m != 0) {
        Int f_trunc = (Int)(trunc(f));
        Int m_trunc = (Int)(trunc(m));

        if (f == f_trunc && m == m_trunc) {
            f = f_trunc % m_trunc;
        } else {
            if (m < 0) {
                m = -m;
            }

            if (f >= m) {
                if (f >= m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f -= m;
                }
            } else if (f <= -m) {
                if (f <= -m * 2.0) {
                    number d = f / m;
                    Int i = (Int)(trunc(d));
                    d = d - i;
                    f = d * m;
                } else {
                    f += m;
                }
            }
        }
    } else {
        f = 0.0;
    }

    return f;
}

number wrap(number x, number low, number high) {
    number lo;
    number hi;

    if (low == high)
        return low;

    if (low > high) {
        hi = low;
        lo = high;
    } else {
        lo = low;
        hi = high;
    }

    number range = hi - lo;

    if (x >= lo && x < hi)
        return x;

    if (range <= 0.000000001)
        return lo;

    Int numWraps = (Int)(trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

template<typename BUFFERTYPE> void poke_boundmode_wrap(
    BUFFERTYPE& buffer,
    SampleValue value,
    SampleValue sampleIndex,
    Int channel,
    number overdub
) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());

    if (bufferSize > 0 && (3 != 5 || (sampleIndex >= 0 && sampleIndex < bufferSize)) && (5 != 5 || (channel >= 0 && channel < bufferChannels))) {
        {
            SampleValue bufferbindindex_result;

            {
                {
                    {
                        bufferbindindex_result = this->wrap(sampleIndex, 0, bufferSize);
                    }
                }
            }

            sampleIndex = bufferbindindex_result;
        }

        if (overdub != 0) {
            number currentValue = buffer->getSample(channel, sampleIndex);

            {
                value = value + currentValue * overdub;
            }
        }

        buffer->setSample(channel, sampleIndex, value);
        buffer->setTouched(true);
    }
}

inline number safediv(number num, number denom) {
    return (denom == 0.0 ? 0.0 : num / denom);
}

number safepow(number base, number exponent) {
    return fixnan(rnbo_pow(base, exponent));
}

number scale(
    number x,
    number lowin,
    number hiin,
    number lowout,
    number highout,
    number pow
) {
    auto inscale = this->safediv(1., hiin - lowin);
    number outdiff = highout - lowout;
    number value = (x - lowin) * inscale;

    if (pow != 1) {
        if (value > 0)
            value = this->safepow(value, pow);
        else
            value = -this->safepow(-value, pow);
    }

    value = value * outdiff + lowout;
    return value;
}

template<typename BUFFERTYPE> void poke_default(
    BUFFERTYPE& buffer,
    SampleValue value,
    SampleValue sampleIndex,
    Int channel,
    number overdub
) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());

    if (bufferSize > 0 && (5 != 5 || (sampleIndex >= 0 && sampleIndex < bufferSize)) && (5 != 5 || (channel >= 0 && channel < bufferChannels))) {
        if (overdub != 0) {
            number currentValue = buffer->getSample(channel, sampleIndex);

            {
                value = value + currentValue * overdub;
            }
        }

        buffer->setSample(channel, sampleIndex, value);
        buffer->setTouched(true);
    }
}

template<typename BUFFERTYPE> array<SampleValue, 1 + 1> wave_boundmode_wrap_interp_cubic(
    BUFFERTYPE& buffer,
    SampleValue phase,
    SampleValue start,
    SampleValue end,
    Int channelOffset
) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());
    constexpr int ___N4 = 1 + 1;
    array<SampleValue, ___N4> out = FIXEDSIZEARRAYINIT(1 + 1);

    if (start < 0)
        start = 0;

    if (end > bufferSize)
        end = bufferSize;

    if (end - start <= 0) {
        start = 0;
        end = bufferSize;
    }

    number sampleIndex;

    {
        SampleValue bufferphasetoindex_result;

        {
            auto __end = end;
            auto __start = start;
            auto __phase = phase;
            number size;

            {
                size = __end - __start;
            }

            {
                {
                    {
                        bufferphasetoindex_result = __start + __phase * size;
                    }
                }
            }
        }

        sampleIndex = bufferphasetoindex_result;
    }

    if (bufferSize == 0 || (3 == 5 && (sampleIndex < start || sampleIndex >= end))) {
        return out;
    } else {
        {
            SampleValue bufferbindindex_result;

            {
                {
                    {
                        bufferbindindex_result = this->wrap(sampleIndex, start, end);
                    }
                }
            }

            sampleIndex = bufferbindindex_result;
        }

        for (Index channel = 0; channel < 1; channel++) {
            Index channelIndex = (Index)(channel + channelOffset);

            {
                Index bufferbindchannel_result;

                {
                    {
                        {
                            bufferbindchannel_result = (bufferChannels == 0 ? 0 : channelIndex % bufferChannels);
                        }
                    }
                }

                channelIndex = bufferbindchannel_result;
            }

            SampleValue bufferreadsample_result;

            {
                auto& __buffer = buffer;

                if (sampleIndex < 0.0) {
                    bufferreadsample_result = 0.0;
                }

                SampleIndex truncIndex = (SampleIndex)(trunc(sampleIndex));

                {
                    number frac = sampleIndex - truncIndex;
                    number wrap = end - 1;

                    {
                        {
                            SampleIndex index1 = (SampleIndex)(truncIndex - 1);

                            if (index1 < 0)
                                index1 = wrap;

                            SampleIndex index2 = (SampleIndex)(index1 + 1);

                            if (index2 > wrap)
                                index2 = start;

                            SampleIndex index3 = (SampleIndex)(index2 + 1);

                            if (index3 > wrap)
                                index3 = start;

                            SampleIndex index4 = (SampleIndex)(index3 + 1);

                            if (index4 > wrap)
                                index4 = start;

                            bufferreadsample_result = this->cubicinterp(
                                frac,
                                __buffer->getSample(channelIndex, index1),
                                __buffer->getSample(channelIndex, index2),
                                __buffer->getSample(channelIndex, index3),
                                __buffer->getSample(channelIndex, index4)
                            );
                        }
                    }
                }
            }

            out[(Index)channel] = bufferreadsample_result;
        }

        out[1] = sampleIndex - start;
        return out;
    }
}

number __wrapped_op_clamp(number in1, number in2, number in3) {
    return (in1 > in3 ? in3 : (in1 < in2 ? in2 : in1));
}

number fromnormalized(Index index, number normalizedValue) {
    return this->convertFromNormalizedParameterValue(index, normalizedValue);
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->gen_01_density_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->gen_01_Mix_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->gen_01_movDur_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->gen_01_grainSize_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->gen_01_feedCoe_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->gen_01_delTime_set(v);
}

void param_07_value_set(number v) {
    v = this->param_07_value_constrain(v);
    this->param_07_value = v;
    this->sendParameter(6, false);

    if (this->param_07_value != this->param_07_lastValue) {
        this->getEngine()->presetTouched();
        this->param_07_lastValue = this->param_07_value;
    }

    this->gen_01_reverse_set(v);
}

MillisecondTime getPatcherTime() const {
    return this->_currentTime;
}

void deallocateSignals() {
    Index i;
    this->globaltransport_tempo = freeSignal(this->globaltransport_tempo);
    this->globaltransport_state = freeSignal(this->globaltransport_state);
    this->zeroBuffer = freeSignal(this->zeroBuffer);
    this->dummyBuffer = freeSignal(this->dummyBuffer);
}

Index getMaxBlockSize() const {
    return this->maxvs;
}

number getSampleRate() const {
    return this->sr;
}

bool hasFixedVectorSize() const {
    return false;
}

void setProbingTarget(MessageTag ) {}

void fillDataRef(DataRefIndex , DataRef& ) {}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void allocateDataRefs() {
    this->data_01_buffer->requestSize(1, 2);
    this->data_01_buffer->setSampleRate(this->sr);
    this->data_01_buffer = this->data_01_buffer->allocateIfNeeded();
    this->gen_01_RingBuffIndex = this->gen_01_RingBuffIndex->allocateIfNeeded();

    if (this->ringBuffIndex->hasRequestedSize()) {
        if (this->ringBuffIndex->wantsFill())
            this->zeroDataRef(this->ringBuffIndex);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->data_02_buffer = this->data_02_buffer->allocateIfNeeded();
    this->gen_01_RingBuff = this->gen_01_RingBuff->allocateIfNeeded();

    if (this->ringBuff->hasRequestedSize()) {
        if (this->ringBuff->wantsFill())
            this->zeroDataRef(this->ringBuff);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_01_delL_buffer = this->gen_01_delL_buffer->allocateIfNeeded();

    if (this->gen_01_delL_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delL_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delL_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }

    this->gen_01_delR_buffer = this->gen_01_delR_buffer->allocateIfNeeded();

    if (this->gen_01_delR_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delR_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delR_bufferobj);

        this->getEngine()->sendDataRefUpdated(3);
    }
}

void initializeObjects() {
    this->data_01_init();
    this->data_02_init();
    this->gen_01_lastPhaseReader1_init();
    this->gen_01_lastPhaseReader2_init();
    this->gen_01_PhaseDiff1_init();
    this->gen_01_PhaseDiff2_init();
    this->gen_01_delL_init();
    this->gen_01_delR_init();
    this->gen_01_counter_1_init();
    this->gen_01_counter_3_init();
    this->gen_01_latch_5_init();
    this->gen_01_latch_7_init();
    this->gen_01_noise_10_init();
    this->gen_01_noise_13_init();
    this->gen_01_noise_20_init();
    this->gen_01_latch_22_init();
    this->gen_01_noise_24_init();
    this->gen_01_latch_26_init();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

void onSampleRateChanged(double ) {}

void extractState(PatcherStateInterface& ) {}

void applyState() {}

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time, (ENGINE*)nullptr);
    this->processOutletAtCurrentTime(sender, index, value);
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime(), (ENGINE*)nullptr);

    {
        this->scheduleParamInit(0, 0);
    }

    {
        this->scheduleParamInit(1, 0);
    }

    {
        this->scheduleParamInit(2, 0);
    }

    {
        this->scheduleParamInit(3, 0);
    }

    {
        this->scheduleParamInit(4, 0);
    }

    {
        this->scheduleParamInit(5, 0);
    }

    {
        this->scheduleParamInit(6, 0);
    }

    this->processParamInitEvents();
}

number param_01_value_constrain(number v) const {
    v = (v > 100 ? 100 : (v < 1 ? 1 : v));
    return v;
}

void gen_01_density_set(number v) {
    this->gen_01_density = v;
}

number param_02_value_constrain(number v) const {
    v = (v > 100 ? 100 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_Mix_set(number v) {
    this->gen_01_Mix = v;
}

number param_03_value_constrain(number v) const {
    v = (v > 1000 ? 1000 : (v < 5 ? 5 : v));
    return v;
}

void gen_01_movDur_set(number v) {
    this->gen_01_movDur = v;
}

number param_04_value_constrain(number v) const {
    v = (v > 2000 ? 2000 : (v < 10 ? 10 : v));
    return v;
}

void gen_01_grainSize_set(number v) {
    this->gen_01_grainSize = v;
}

number param_05_value_constrain(number v) const {
    v = (v > 0.9 ? 0.9 : (v < 0.01 ? 0.01 : v));
    return v;
}

void gen_01_feedCoe_set(number v) {
    this->gen_01_feedCoe = v;
}

number param_06_value_constrain(number v) const {
    v = (v > 2500 ? 2500 : (v < 2 ? 2 : v));
    return v;
}

void gen_01_delTime_set(number v) {
    this->gen_01_delTime = v;
}

number param_07_value_constrain(number v) const {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_reverse_set(number v) {
    this->gen_01_reverse = v;
}

void ctlin_01_outchannel_set(number ) {}

void ctlin_01_outcontroller_set(number ) {}

void fromnormalized_01_output_set(number v) {
    this->param_02_value_set(v);
}

void fromnormalized_01_input_set(number v) {
    this->fromnormalized_01_output_set(this->fromnormalized(1, v));
}

void expr_01_out1_set(number v) {
    this->expr_01_out1 = v;
    this->fromnormalized_01_input_set(this->expr_01_out1);
}

void expr_01_in1_set(number in1) {
    this->expr_01_in1 = in1;
    this->expr_01_out1_set(this->expr_01_in1 * this->expr_01_in2);//#map:expr_01:1
}

void ctlin_01_value_set(number v) {
    this->expr_01_in1_set(v);
}

void ctlin_01_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_01_channel || this->ctlin_01_channel == -1) && (data[1] == this->ctlin_01_controller || this->ctlin_01_controller == -1)) {
        this->ctlin_01_outchannel_set(channel);
        this->ctlin_01_outcontroller_set(data[1]);
        this->ctlin_01_value_set(data[2]);
        this->ctlin_01_status = 0;
    }
}

void ctlin_02_outchannel_set(number ) {}

void ctlin_02_outcontroller_set(number ) {}

void fromnormalized_02_output_set(number v) {
    this->param_03_value_set(v);
}

void fromnormalized_02_input_set(number v) {
    this->fromnormalized_02_output_set(this->fromnormalized(2, v));
}

void expr_02_out1_set(number v) {
    this->expr_02_out1 = v;
    this->fromnormalized_02_input_set(this->expr_02_out1);
}

void expr_02_in1_set(number in1) {
    this->expr_02_in1 = in1;
    this->expr_02_out1_set(this->expr_02_in1 * this->expr_02_in2);//#map:expr_02:1
}

void ctlin_02_value_set(number v) {
    this->expr_02_in1_set(v);
}

void ctlin_02_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_02_channel || this->ctlin_02_channel == -1) && (data[1] == this->ctlin_02_controller || this->ctlin_02_controller == -1)) {
        this->ctlin_02_outchannel_set(channel);
        this->ctlin_02_outcontroller_set(data[1]);
        this->ctlin_02_value_set(data[2]);
        this->ctlin_02_status = 0;
    }
}

void ctlin_03_outchannel_set(number ) {}

void ctlin_03_outcontroller_set(number ) {}

void fromnormalized_03_output_set(number v) {
    this->param_04_value_set(v);
}

void fromnormalized_03_input_set(number v) {
    this->fromnormalized_03_output_set(this->fromnormalized(3, v));
}

void expr_03_out1_set(number v) {
    this->expr_03_out1 = v;
    this->fromnormalized_03_input_set(this->expr_03_out1);
}

void expr_03_in1_set(number in1) {
    this->expr_03_in1 = in1;
    this->expr_03_out1_set(this->expr_03_in1 * this->expr_03_in2);//#map:expr_03:1
}

void ctlin_03_value_set(number v) {
    this->expr_03_in1_set(v);
}

void ctlin_03_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_03_channel || this->ctlin_03_channel == -1) && (data[1] == this->ctlin_03_controller || this->ctlin_03_controller == -1)) {
        this->ctlin_03_outchannel_set(channel);
        this->ctlin_03_outcontroller_set(data[1]);
        this->ctlin_03_value_set(data[2]);
        this->ctlin_03_status = 0;
    }
}

void ctlin_04_outchannel_set(number ) {}

void ctlin_04_outcontroller_set(number ) {}

void fromnormalized_04_output_set(number v) {
    this->param_05_value_set(v);
}

void fromnormalized_04_input_set(number v) {
    this->fromnormalized_04_output_set(this->fromnormalized(4, v));
}

void expr_04_out1_set(number v) {
    this->expr_04_out1 = v;
    this->fromnormalized_04_input_set(this->expr_04_out1);
}

void expr_04_in1_set(number in1) {
    this->expr_04_in1 = in1;
    this->expr_04_out1_set(this->expr_04_in1 * this->expr_04_in2);//#map:expr_04:1
}

void ctlin_04_value_set(number v) {
    this->expr_04_in1_set(v);
}

void ctlin_04_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_04_channel || this->ctlin_04_channel == -1) && (data[1] == this->ctlin_04_controller || this->ctlin_04_controller == -1)) {
        this->ctlin_04_outchannel_set(channel);
        this->ctlin_04_outcontroller_set(data[1]);
        this->ctlin_04_value_set(data[2]);
        this->ctlin_04_status = 0;
    }
}

void ctlin_05_outchannel_set(number ) {}

void ctlin_05_outcontroller_set(number ) {}

void fromnormalized_05_output_set(number v) {
    this->param_06_value_set(v);
}

void fromnormalized_05_input_set(number v) {
    this->fromnormalized_05_output_set(this->fromnormalized(5, v));
}

void expr_05_out1_set(number v) {
    this->expr_05_out1 = v;
    this->fromnormalized_05_input_set(this->expr_05_out1);
}

void expr_05_in1_set(number in1) {
    this->expr_05_in1 = in1;
    this->expr_05_out1_set(this->expr_05_in1 * this->expr_05_in2);//#map:expr_05:1
}

void ctlin_05_value_set(number v) {
    this->expr_05_in1_set(v);
}

void ctlin_05_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_05_channel || this->ctlin_05_channel == -1) && (data[1] == this->ctlin_05_controller || this->ctlin_05_controller == -1)) {
        this->ctlin_05_outchannel_set(channel);
        this->ctlin_05_outcontroller_set(data[1]);
        this->ctlin_05_value_set(data[2]);
        this->ctlin_05_status = 0;
    }
}

void ctlin_06_outchannel_set(number ) {}

void ctlin_06_outcontroller_set(number ) {}

void fromnormalized_06_output_set(number v) {
    this->param_07_value_set(v);
}

void fromnormalized_06_input_set(number v) {
    this->fromnormalized_06_output_set(this->fromnormalized(6, v));
}

void expr_06_out1_set(number v) {
    this->expr_06_out1 = v;
    this->fromnormalized_06_input_set(this->expr_06_out1);
}

void expr_06_in1_set(number in1) {
    this->expr_06_in1 = in1;
    this->expr_06_out1_set(this->expr_06_in1 * this->expr_06_in2);//#map:expr_06:1
}

void ctlin_06_value_set(number v) {
    this->expr_06_in1_set(v);
}

void ctlin_06_midihandler(int status, int channel, int port, ConstByteArray data, Index length) {
    RNBO_UNUSED(length);
    RNBO_UNUSED(port);

    if (status == 0xB0 && (channel == this->ctlin_06_channel || this->ctlin_06_channel == -1) && (data[1] == this->ctlin_06_controller || this->ctlin_06_controller == -1)) {
        this->ctlin_06_outchannel_set(channel);
        this->ctlin_06_outcontroller_set(data[1]);
        this->ctlin_06_value_set(data[2]);
        this->ctlin_06_status = 0;
    }
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    number movDur,
    number grainSize,
    number reverse,
    number Mix,
    number delTime,
    number feedCoe,
    number density,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_01_lastPhaseReader2_value = this->gen_01_lastPhaseReader2_value;
    auto __gen_01_lastPhaseReader1_value = this->gen_01_lastPhaseReader1_value;
    auto __gen_01_PhaseDiff2_value = this->gen_01_PhaseDiff2_value;
    auto __gen_01_PhaseDiff1_value = this->gen_01_PhaseDiff1_value;
    number grainSpeed_0 = (grainSize == 0. ? 0. : (number)1000 / grainSize);
    Index i;

    for (i = 0; i < (Index)n; i++) {
        number BuffCount1 = 0;
        number BuffCount2 = 0;
        number BuffCount3 = 0;
        array<number, 3> result_2 = this->gen_01_counter_1_next(1, 0, this->dim(this->gen_01_RingBuff));
        BuffCount3 = result_2[2];
        BuffCount2 = result_2[1];
        BuffCount1 = result_2[0];
        this->poke_boundmode_wrap(this->gen_01_RingBuff, in1[(Index)i], BuffCount1, 0, 0);
        this->poke_boundmode_wrap(this->gen_01_RingBuff, in2[(Index)i], BuffCount1, 1, 0);
        number MovCount1 = 0;
        number MovCount2 = 0;
        number MovCount3 = 0;
        array<number, 3> result_4 = this->gen_01_counter_3_next(1, 0, this->mstosamps(movDur));
        MovCount3 = result_4[2];
        MovCount2 = result_4[1];
        MovCount1 = result_4[0];
        number holdGrainSize1_6 = this->gen_01_latch_5_next(this->mstosamps(grainSize), __gen_01_PhaseDiff1_value >= 0.99);
        number holdGrainSize2_8 = this->gen_01_latch_7_next(this->mstosamps(grainSize), __gen_01_PhaseDiff2_value >= 0.99);

        auto pos1_11 = this->scale(
            this->gen_01_sah_9_next(this->gen_01_noise_10_next(), MovCount1, 0),
            -1,
            1,
            0,
            this->dim(this->gen_01_RingBuff) - holdGrainSize1_6,
            1
        );

        auto pos2_14 = this->scale(
            this->gen_01_sah_12_next(this->gen_01_noise_13_next(), MovCount1, 0),
            -1,
            1,
            0,
            this->dim(this->gen_01_RingBuff) - holdGrainSize1_6,
            1
        );

        number moving_window_15 = 0.5 - 0.5 * rnbo_cos(
            6.28318530717958647692 * ((this->mstosamps(movDur) == 0. ? 0. : MovCount1 / this->mstosamps(movDur)))
        );

        number phaseReader1_17 = this->gen_01_phasor_16_next(grainSpeed_0, 0);
        auto phaseReader2_19 = this->wrap(this->gen_01_phasor_18_next(grainSpeed_0, 0) + 0.5, 0, 1);
        __gen_01_PhaseDiff1_value = __gen_01_lastPhaseReader1_value - phaseReader1_17;
        __gen_01_PhaseDiff2_value = __gen_01_lastPhaseReader2_value - phaseReader2_19;
        __gen_01_lastPhaseReader1_value = phaseReader1_17;
        __gen_01_lastPhaseReader2_value = phaseReader2_19;
        number randDensity1_21 = (this->gen_01_noise_20_next() + 1) * 0.5;
        number flagWindow1_23 = this->gen_01_latch_22_next(randDensity1_21, __gen_01_PhaseDiff1_value >= 0.99) < density * 0.01;
        number randDensity2_25 = (this->gen_01_noise_24_next() + 1) * 0.5;
        number flagWindow2_27 = this->gen_01_latch_26_next(randDensity2_25, __gen_01_PhaseDiff2_value >= 0.99) < density * 0.01;
        number grain_window1_28 = (0.5 - 0.5 * rnbo_cos(6.28318530717958647692 * phaseReader1_17)) * flagWindow1_23;
        number grain_window2_29 = (0.5 - 0.5 * rnbo_cos(6.28318530717958647692 * phaseReader2_19)) * flagWindow2_27;

        if (reverse == 1) {
            phaseReader1_17 = 1 - phaseReader1_17;
            phaseReader2_19 = 1 - phaseReader2_19;
        }

        auto buffIndex1_30 = this->scale(phaseReader1_17, 0, 1, pos1_11, pos1_11 + holdGrainSize1_6, 1);
        auto buffIndex2_31 = this->scale(phaseReader2_19, 0, 1, pos2_14, pos2_14 + holdGrainSize2_8, 1);
        auto wrapedIndex1_32 = this->wrap(buffIndex1_30, 0, this->dim(this->gen_01_RingBuff));
        auto wrapedIndex2_33 = this->wrap(buffIndex2_31, 0, this->dim(this->gen_01_RingBuff));
        this->poke_default(this->gen_01_RingBuffIndex, wrapedIndex1_32 * flagWindow1_23, 0, 0, 0);
        this->poke_default(this->gen_01_RingBuffIndex, wrapedIndex2_33 * flagWindow2_27, 0, 1, 0);
        number grainL = 0;
        number waveIndex1 = 0;

        auto result_34 = this->wave_boundmode_wrap_interp_cubic(
            this->gen_01_RingBuff,
            phaseReader1_17,
            pos1_11,
            pos1_11 + holdGrainSize1_6,
            0
        );

        waveIndex1 = result_34[1];
        grainL = result_34[0];
        number grainR = 0;
        number waveIndex2 = 0;

        auto result_35 = this->wave_boundmode_wrap_interp_cubic(
            this->gen_01_RingBuff,
            phaseReader2_19,
            pos2_14,
            pos2_14 + holdGrainSize2_8,
            1
        );

        waveIndex2 = result_35[1];
        grainR = result_35[0];
        number windowGrainL_36 = grainL * moving_window_15 * grain_window1_28;
        number windowGrainR_37 = grainR * moving_window_15 * grain_window2_29;
        number delGrainL_38 = this->gen_01_delL_read(delTime, 1);
        number feedGrainL_39 = delGrainL_38 * feedCoe + windowGrainL_36;
        this->gen_01_delL_write(windowGrainL_36 + feedGrainL_39);
        number delGrainR_40 = this->gen_01_delR_read(delTime, 1);
        number feedGrainR_41 = delGrainR_40 * feedCoe + windowGrainR_37;
        this->gen_01_delR_write(windowGrainR_37 + feedGrainR_41);

        number expr_1_43 = this->__wrapped_op_clamp(
            in1[(Index)i] + Mix * 0.01 * (this->gen_01_dcblock_42_next(feedGrainL_39 * 0.8, 0.9997) - in1[(Index)i]),
            -1,
            1
        );

        number expr_2_45 = this->__wrapped_op_clamp(
            in2[(Index)i] + Mix * 0.01 * (this->gen_01_dcblock_44_next(feedGrainR_41 * 0.8, 0.9997) - in2[(Index)i]),
            -1,
            1
        );

        out2[(Index)i] = expr_2_45;
        out1[(Index)i] = expr_1_43;
        this->gen_01_delL_step();
        this->gen_01_delR_step();
    }

    this->gen_01_PhaseDiff1_value = __gen_01_PhaseDiff1_value;
    this->gen_01_PhaseDiff2_value = __gen_01_PhaseDiff2_value;
    this->gen_01_lastPhaseReader1_value = __gen_01_lastPhaseReader1_value;
    this->gen_01_lastPhaseReader2_value = __gen_01_lastPhaseReader2_value;
}

void stackprotect_perform(Index n) {
    RNBO_UNUSED(n);
    auto __stackprotect_count = this->stackprotect_count;
    __stackprotect_count = 0;
    this->stackprotect_count = __stackprotect_count;
}

void data_01_srout_set(number ) {}

void data_01_chanout_set(number ) {}

void data_01_sizeout_set(number v) {
    this->data_01_sizeout = v;
}

void data_02_srout_set(number ) {}

void data_02_chanout_set(number ) {}

void data_02_sizeout_set(number v) {
    this->data_02_sizeout = v;
}

void data_01_init() {
    this->data_01_buffer->setWantsFill(true);
}

Index data_01_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    number size = 0;
    return (Index)(size);
}

void data_01_dspsetup(bool force) {
    if ((bool)(this->data_01_setupDone) && (bool)(!(bool)(force)))
        return;

    if (this->data_01_sizemode == 2) {
        this->data_01_buffer = this->data_01_buffer->setSize((Index)(this->mstosamps(this->data_01_sizems)));
        updateDataRef(this, this->data_01_buffer);
    } else if (this->data_01_sizemode == 3) {
        this->data_01_buffer = this->data_01_buffer->setSize(this->data_01_evaluateSizeExpr(this->sr, this->vs));
        updateDataRef(this, this->data_01_buffer);
    }

    this->data_01_setupDone = true;
}

void data_01_bufferUpdated() {
    this->data_01_report();
}

void data_01_report() {
    this->data_01_srout_set(this->data_01_buffer->getSampleRate());
    this->data_01_chanout_set(this->data_01_buffer->getChannels());
    this->data_01_sizeout_set(this->data_01_buffer->getSize());
}

void data_02_init() {
    {
        this->data_02_buffer->requestSize(this->data_02_evaluateSizeExpr(this->sr, this->vs), this->data_02_channels);
    }

    this->data_02_buffer->setWantsFill(true);
}

Index data_02_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    number size = 0;

    {
        size = samplerate * 2;
    }

    return (Index)(size);
}

void data_02_dspsetup(bool force) {
    if ((bool)(this->data_02_setupDone) && (bool)(!(bool)(force)))
        return;

    if (this->data_02_sizemode == 2) {
        this->data_02_buffer = this->data_02_buffer->setSize((Index)(this->mstosamps(this->data_02_sizems)));
        updateDataRef(this, this->data_02_buffer);
    } else if (this->data_02_sizemode == 3) {
        this->data_02_buffer = this->data_02_buffer->setSize(this->data_02_evaluateSizeExpr(this->sr, this->vs));
        updateDataRef(this, this->data_02_buffer);
    }

    this->data_02_setupDone = true;
}

void data_02_bufferUpdated() {
    this->data_02_report();
}

void data_02_report() {
    this->data_02_srout_set(this->data_02_buffer->getSampleRate());
    this->data_02_chanout_set(this->data_02_buffer->getChannels());
    this->data_02_sizeout_set(this->data_02_buffer->getSize());
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
}

number gen_01_lastPhaseReader1_getvalue() {
    return this->gen_01_lastPhaseReader1_value;
}

void gen_01_lastPhaseReader1_setvalue(number val) {
    this->gen_01_lastPhaseReader1_value = val;
}

void gen_01_lastPhaseReader1_reset() {
    this->gen_01_lastPhaseReader1_value = 0;
}

void gen_01_lastPhaseReader1_init() {
    this->gen_01_lastPhaseReader1_value = 0;
}

number gen_01_lastPhaseReader2_getvalue() {
    return this->gen_01_lastPhaseReader2_value;
}

void gen_01_lastPhaseReader2_setvalue(number val) {
    this->gen_01_lastPhaseReader2_value = val;
}

void gen_01_lastPhaseReader2_reset() {
    this->gen_01_lastPhaseReader2_value = 0;
}

void gen_01_lastPhaseReader2_init() {
    this->gen_01_lastPhaseReader2_value = 0;
}

number gen_01_PhaseDiff1_getvalue() {
    return this->gen_01_PhaseDiff1_value;
}

void gen_01_PhaseDiff1_setvalue(number val) {
    this->gen_01_PhaseDiff1_value = val;
}

void gen_01_PhaseDiff1_reset() {
    this->gen_01_PhaseDiff1_value = 0;
}

void gen_01_PhaseDiff1_init() {
    this->gen_01_PhaseDiff1_value = 0;
}

number gen_01_PhaseDiff2_getvalue() {
    return this->gen_01_PhaseDiff2_value;
}

void gen_01_PhaseDiff2_setvalue(number val) {
    this->gen_01_PhaseDiff2_value = val;
}

void gen_01_PhaseDiff2_reset() {
    this->gen_01_PhaseDiff2_value = 0;
}

void gen_01_PhaseDiff2_init() {
    this->gen_01_PhaseDiff2_value = 0;
}

void gen_01_delL_step() {
    this->gen_01_delL_reader++;

    if (this->gen_01_delL_reader >= (Int)(this->gen_01_delL_buffer->getSize()))
        this->gen_01_delL_reader = 0;
}

number gen_01_delL_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        {
            number r = (Int)(this->gen_01_delL_buffer->getSize()) + this->gen_01_delL_reader - ((size > this->gen_01_delL__maxdelay ? this->gen_01_delL__maxdelay : (size < (1 + this->gen_01_delL_reader != this->gen_01_delL_writer) ? 1 + this->gen_01_delL_reader != this->gen_01_delL_writer : size)));
            Int index1 = (Int)(rnbo_floor(r));
            number frac = r - index1;
            Index index2 = (Index)(index1 + 1);
            Index index3 = (Index)(index2 + 1);
            Index index4 = (Index)(index3 + 1);

            return this->cubicinterp(frac, this->gen_01_delL_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delL_wrap))
            ), this->gen_01_delL_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_delL_wrap))
            ), this->gen_01_delL_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index3 & (BinOpInt)this->gen_01_delL_wrap))
            ), this->gen_01_delL_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index4 & (BinOpInt)this->gen_01_delL_wrap))
            ));
        }
    }

    number r = (Int)(this->gen_01_delL_buffer->getSize()) + this->gen_01_delL_reader - ((size > this->gen_01_delL__maxdelay ? this->gen_01_delL__maxdelay : (size < (this->gen_01_delL_reader != this->gen_01_delL_writer) ? this->gen_01_delL_reader != this->gen_01_delL_writer : size)));
    Int index1 = (Int)(rnbo_floor(r));

    return this->gen_01_delL_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delL_wrap))
    );
}

void gen_01_delL_write(number v) {
    this->gen_01_delL_writer = this->gen_01_delL_reader;
    this->gen_01_delL_buffer[(Index)this->gen_01_delL_writer] = v;
}

number gen_01_delL_next(number v, Int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delL__maxdelay : size);
    number val = this->gen_01_delL_read(effectiveSize, 0);
    this->gen_01_delL_write(v);
    this->gen_01_delL_step();
    return val;
}

array<Index, 2> gen_01_delL_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delL_evaluateSizeExpr(this->sr, this->vs);
        this->gen_01_delL_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delL_init() {
    auto result = this->gen_01_delL_calcSizeInSamples();
    this->gen_01_delL__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delL_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delL_wrap = requestedSizeInSamples - 1;
}

void gen_01_delL_clear() {
    this->gen_01_delL_buffer->setZero();
}

void gen_01_delL_reset() {
    auto result = this->gen_01_delL_calcSizeInSamples();
    this->gen_01_delL__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delL_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delL_buffer);
    this->gen_01_delL_wrap = this->gen_01_delL_buffer->getSize() - 1;
    this->gen_01_delL_clear();

    if (this->gen_01_delL_reader >= this->gen_01_delL__maxdelay || this->gen_01_delL_writer >= this->gen_01_delL__maxdelay) {
        this->gen_01_delL_reader = 0;
        this->gen_01_delL_writer = 0;
    }
}

void gen_01_delL_dspsetup() {
    this->gen_01_delL_reset();
}

number gen_01_delL_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->sr;
}

number gen_01_delL_size() {
    return this->gen_01_delL__maxdelay;
}

void gen_01_delR_step() {
    this->gen_01_delR_reader++;

    if (this->gen_01_delR_reader >= (Int)(this->gen_01_delR_buffer->getSize()))
        this->gen_01_delR_reader = 0;
}

number gen_01_delR_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        {
            number r = (Int)(this->gen_01_delR_buffer->getSize()) + this->gen_01_delR_reader - ((size > this->gen_01_delR__maxdelay ? this->gen_01_delR__maxdelay : (size < (1 + this->gen_01_delR_reader != this->gen_01_delR_writer) ? 1 + this->gen_01_delR_reader != this->gen_01_delR_writer : size)));
            Int index1 = (Int)(rnbo_floor(r));
            number frac = r - index1;
            Index index2 = (Index)(index1 + 1);
            Index index3 = (Index)(index2 + 1);
            Index index4 = (Index)(index3 + 1);

            return this->cubicinterp(frac, this->gen_01_delR_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delR_wrap))
            ), this->gen_01_delR_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index2 & (BinOpInt)this->gen_01_delR_wrap))
            ), this->gen_01_delR_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index3 & (BinOpInt)this->gen_01_delR_wrap))
            ), this->gen_01_delR_buffer->getSample(
                0,
                (Index)((BinOpInt)((BinOpInt)index4 & (BinOpInt)this->gen_01_delR_wrap))
            ));
        }
    }

    number r = (Int)(this->gen_01_delR_buffer->getSize()) + this->gen_01_delR_reader - ((size > this->gen_01_delR__maxdelay ? this->gen_01_delR__maxdelay : (size < (this->gen_01_delR_reader != this->gen_01_delR_writer) ? this->gen_01_delR_reader != this->gen_01_delR_writer : size)));
    Int index1 = (Int)(rnbo_floor(r));

    return this->gen_01_delR_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delR_wrap))
    );
}

void gen_01_delR_write(number v) {
    this->gen_01_delR_writer = this->gen_01_delR_reader;
    this->gen_01_delR_buffer[(Index)this->gen_01_delR_writer] = v;
}

number gen_01_delR_next(number v, Int size) {
    number effectiveSize = (size == -1 ? this->gen_01_delR__maxdelay : size);
    number val = this->gen_01_delR_read(effectiveSize, 0);
    this->gen_01_delR_write(v);
    this->gen_01_delR_step();
    return val;
}

array<Index, 2> gen_01_delR_calcSizeInSamples() {
    number sizeInSamples = 0;
    Index allocatedSizeInSamples = 0;

    {
        sizeInSamples = this->gen_01_delR_evaluateSizeExpr(this->sr, this->vs);
        this->gen_01_delR_sizemode = 0;
    }

    sizeInSamples = rnbo_floor(sizeInSamples);
    sizeInSamples = this->maximum(sizeInSamples, 2);
    allocatedSizeInSamples = (Index)(sizeInSamples);
    allocatedSizeInSamples = nextpoweroftwo(allocatedSizeInSamples);
    return {sizeInSamples, allocatedSizeInSamples};
}

void gen_01_delR_init() {
    auto result = this->gen_01_delR_calcSizeInSamples();
    this->gen_01_delR__maxdelay = result[0];
    Index requestedSizeInSamples = (Index)(result[1]);
    this->gen_01_delR_buffer->requestSize(requestedSizeInSamples, 1);
    this->gen_01_delR_wrap = requestedSizeInSamples - 1;
}

void gen_01_delR_clear() {
    this->gen_01_delR_buffer->setZero();
}

void gen_01_delR_reset() {
    auto result = this->gen_01_delR_calcSizeInSamples();
    this->gen_01_delR__maxdelay = result[0];
    Index allocatedSizeInSamples = (Index)(result[1]);
    this->gen_01_delR_buffer->setSize(allocatedSizeInSamples);
    updateDataRef(this, this->gen_01_delR_buffer);
    this->gen_01_delR_wrap = this->gen_01_delR_buffer->getSize() - 1;
    this->gen_01_delR_clear();

    if (this->gen_01_delR_reader >= this->gen_01_delR__maxdelay || this->gen_01_delR_writer >= this->gen_01_delR__maxdelay) {
        this->gen_01_delR_reader = 0;
        this->gen_01_delR_writer = 0;
    }
}

void gen_01_delR_dspsetup() {
    this->gen_01_delR_reset();
}

number gen_01_delR_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    RNBO_UNUSED(samplerate);
    return this->sr;
}

number gen_01_delR_size() {
    return this->gen_01_delR__maxdelay;
}

array<number, 3> gen_01_counter_1_next(number a, number reset, number limit) {
    RNBO_UNUSED(reset);
    RNBO_UNUSED(a);
    number carry_flag = 0;

    {
        this->gen_01_counter_1_count += 1;

        if (limit != 0) {
            if ((1 > 0 && this->gen_01_counter_1_count >= limit) || (1 < 0 && this->gen_01_counter_1_count <= limit)) {
                this->gen_01_counter_1_count = 0;
                this->gen_01_counter_1_carry += 1;
                carry_flag = 1;
            }
        }
    }

    return {this->gen_01_counter_1_count, carry_flag, this->gen_01_counter_1_carry};
}

void gen_01_counter_1_init() {
    this->gen_01_counter_1_count = 0;
}

void gen_01_counter_1_reset() {
    this->gen_01_counter_1_carry = 0;
    this->gen_01_counter_1_count = 0;
}

array<number, 3> gen_01_counter_3_next(number a, number reset, number limit) {
    RNBO_UNUSED(reset);
    RNBO_UNUSED(a);
    number carry_flag = 0;

    {
        this->gen_01_counter_3_count += 1;

        if (limit != 0) {
            if ((1 > 0 && this->gen_01_counter_3_count >= limit) || (1 < 0 && this->gen_01_counter_3_count <= limit)) {
                this->gen_01_counter_3_count = 0;
                this->gen_01_counter_3_carry += 1;
                carry_flag = 1;
            }
        }
    }

    return {this->gen_01_counter_3_count, carry_flag, this->gen_01_counter_3_carry};
}

void gen_01_counter_3_init() {
    this->gen_01_counter_3_count = 0;
}

void gen_01_counter_3_reset() {
    this->gen_01_counter_3_carry = 0;
    this->gen_01_counter_3_count = 0;
}

number gen_01_latch_5_next(number x, number control) {
    if (control != 0.) {
        this->gen_01_latch_5_value = x;
    }

    return this->gen_01_latch_5_value;
}

void gen_01_latch_5_dspsetup() {
    this->gen_01_latch_5_reset();
}

void gen_01_latch_5_init() {
    this->gen_01_latch_5_reset();
}

void gen_01_latch_5_reset() {
    this->gen_01_latch_5_value = 0;
}

number gen_01_latch_7_next(number x, number control) {
    if (control != 0.) {
        this->gen_01_latch_7_value = x;
    }

    return this->gen_01_latch_7_value;
}

void gen_01_latch_7_dspsetup() {
    this->gen_01_latch_7_reset();
}

void gen_01_latch_7_init() {
    this->gen_01_latch_7_reset();
}

void gen_01_latch_7_reset() {
    this->gen_01_latch_7_value = 0;
}

number gen_01_sah_9_next(number x, number trig, number thresh) {
    RNBO_UNUSED(thresh);

    if (this->gen_01_sah_9_prev <= 0 && trig > 0)
        this->gen_01_sah_9_value = x;

    this->gen_01_sah_9_prev = trig;
    return this->gen_01_sah_9_value;
}

void gen_01_sah_9_reset() {
    this->gen_01_sah_9_prev = 0.;
    this->gen_01_sah_9_value = 0.;
}

void gen_01_noise_10_reset() {
    xoshiro_reset(
        systemticks() + this->voice() + this->random(0, 10000),
        this->gen_01_noise_10_state
    );
}

void gen_01_noise_10_init() {
    this->gen_01_noise_10_reset();
}

void gen_01_noise_10_seed(number v) {
    xoshiro_reset(v, this->gen_01_noise_10_state);
}

number gen_01_noise_10_next() {
    return xoshiro_next(this->gen_01_noise_10_state);
}

number gen_01_sah_12_next(number x, number trig, number thresh) {
    RNBO_UNUSED(thresh);

    if (this->gen_01_sah_12_prev <= 0 && trig > 0)
        this->gen_01_sah_12_value = x;

    this->gen_01_sah_12_prev = trig;
    return this->gen_01_sah_12_value;
}

void gen_01_sah_12_reset() {
    this->gen_01_sah_12_prev = 0.;
    this->gen_01_sah_12_value = 0.;
}

void gen_01_noise_13_reset() {
    xoshiro_reset(
        systemticks() + this->voice() + this->random(0, 10000),
        this->gen_01_noise_13_state
    );
}

void gen_01_noise_13_init() {
    this->gen_01_noise_13_reset();
}

void gen_01_noise_13_seed(number v) {
    xoshiro_reset(v, this->gen_01_noise_13_state);
}

number gen_01_noise_13_next() {
    return xoshiro_next(this->gen_01_noise_13_state);
}

number gen_01_phasor_16_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_01_phasor_16_conv;

    if (this->gen_01_phasor_16_currentPhase < 0.)
        this->gen_01_phasor_16_currentPhase = 1. + this->gen_01_phasor_16_currentPhase;

    if (this->gen_01_phasor_16_currentPhase > 1.)
        this->gen_01_phasor_16_currentPhase = this->gen_01_phasor_16_currentPhase - 1.;

    number tmp = this->gen_01_phasor_16_currentPhase;
    this->gen_01_phasor_16_currentPhase += pincr;
    return tmp;
}

void gen_01_phasor_16_reset() {
    this->gen_01_phasor_16_currentPhase = 0;
}

void gen_01_phasor_16_dspsetup() {
    this->gen_01_phasor_16_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

number gen_01_phasor_18_next(number freq, number reset) {
    RNBO_UNUSED(reset);
    number pincr = freq * this->gen_01_phasor_18_conv;

    if (this->gen_01_phasor_18_currentPhase < 0.)
        this->gen_01_phasor_18_currentPhase = 1. + this->gen_01_phasor_18_currentPhase;

    if (this->gen_01_phasor_18_currentPhase > 1.)
        this->gen_01_phasor_18_currentPhase = this->gen_01_phasor_18_currentPhase - 1.;

    number tmp = this->gen_01_phasor_18_currentPhase;
    this->gen_01_phasor_18_currentPhase += pincr;
    return tmp;
}

void gen_01_phasor_18_reset() {
    this->gen_01_phasor_18_currentPhase = 0;
}

void gen_01_phasor_18_dspsetup() {
    this->gen_01_phasor_18_conv = (this->sr == 0. ? 0. : (number)1 / this->sr);
}

void gen_01_noise_20_reset() {
    xoshiro_reset(
        systemticks() + this->voice() + this->random(0, 10000),
        this->gen_01_noise_20_state
    );
}

void gen_01_noise_20_init() {
    this->gen_01_noise_20_reset();
}

void gen_01_noise_20_seed(number v) {
    xoshiro_reset(v, this->gen_01_noise_20_state);
}

number gen_01_noise_20_next() {
    return xoshiro_next(this->gen_01_noise_20_state);
}

number gen_01_latch_22_next(number x, number control) {
    if (control != 0.) {
        this->gen_01_latch_22_value = x;
    }

    return this->gen_01_latch_22_value;
}

void gen_01_latch_22_dspsetup() {
    this->gen_01_latch_22_reset();
}

void gen_01_latch_22_init() {
    this->gen_01_latch_22_reset();
}

void gen_01_latch_22_reset() {
    this->gen_01_latch_22_value = 0;
}

void gen_01_noise_24_reset() {
    xoshiro_reset(
        systemticks() + this->voice() + this->random(0, 10000),
        this->gen_01_noise_24_state
    );
}

void gen_01_noise_24_init() {
    this->gen_01_noise_24_reset();
}

void gen_01_noise_24_seed(number v) {
    xoshiro_reset(v, this->gen_01_noise_24_state);
}

number gen_01_noise_24_next() {
    return xoshiro_next(this->gen_01_noise_24_state);
}

number gen_01_latch_26_next(number x, number control) {
    if (control != 0.) {
        this->gen_01_latch_26_value = x;
    }

    return this->gen_01_latch_26_value;
}

void gen_01_latch_26_dspsetup() {
    this->gen_01_latch_26_reset();
}

void gen_01_latch_26_init() {
    this->gen_01_latch_26_reset();
}

void gen_01_latch_26_reset() {
    this->gen_01_latch_26_value = 0;
}

number gen_01_dcblock_42_next(number x, number gain) {
    RNBO_UNUSED(gain);
    number y = x - this->gen_01_dcblock_42_xm1 + this->gen_01_dcblock_42_ym1 * 0.9997;
    this->gen_01_dcblock_42_xm1 = x;
    this->gen_01_dcblock_42_ym1 = y;
    return y;
}

void gen_01_dcblock_42_reset() {
    this->gen_01_dcblock_42_xm1 = 0;
    this->gen_01_dcblock_42_ym1 = 0;
}

void gen_01_dcblock_42_dspsetup() {
    this->gen_01_dcblock_42_reset();
}

number gen_01_dcblock_44_next(number x, number gain) {
    RNBO_UNUSED(gain);
    number y = x - this->gen_01_dcblock_44_xm1 + this->gen_01_dcblock_44_ym1 * 0.9997;
    this->gen_01_dcblock_44_xm1 = x;
    this->gen_01_dcblock_44_ym1 = y;
    return y;
}

void gen_01_dcblock_44_reset() {
    this->gen_01_dcblock_44_xm1 = 0;
    this->gen_01_dcblock_44_ym1 = 0;
}

void gen_01_dcblock_44_dspsetup() {
    this->gen_01_dcblock_44_reset();
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_delL_dspsetup();
    this->gen_01_delR_dspsetup();
    this->gen_01_latch_5_dspsetup();
    this->gen_01_latch_7_dspsetup();
    this->gen_01_phasor_16_dspsetup();
    this->gen_01_phasor_18_dspsetup();
    this->gen_01_latch_22_dspsetup();
    this->gen_01_latch_26_dspsetup();
    this->gen_01_dcblock_42_dspsetup();
    this->gen_01_dcblock_44_dspsetup();
}

void param_02_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_02_value;
}

void param_02_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_02_value_set(preset["value"]);
}

void param_03_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_03_value;
}

void param_03_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_03_value_set(preset["value"]);
}

void param_04_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_04_value;
}

void param_04_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_04_value_set(preset["value"]);
}

void param_05_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_05_value;
}

void param_05_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_05_value_set(preset["value"]);
}

void param_06_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_06_value;
}

void param_06_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_06_value_set(preset["value"]);
}

void param_07_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_07_value;
}

void param_07_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_07_value_set(preset["value"]);
}

void globaltransport_advance() {}

void globaltransport_dspsetup(bool ) {}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

Index getPatcherSerial() const {
    return 0;
}

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
}

void scheduleParamInit(ParameterIndex index, Index order) {
    this->paramInitIndices->push(index);
    this->paramInitOrder->push(order);
}

void processParamInitEvents() {
    this->listquicksort(
        this->paramInitOrder,
        this->paramInitIndices,
        0,
        (int)(this->paramInitOrder->length - 1),
        true
    );

    for (Index i = 0; i < this->paramInitOrder->length; i++) {
        this->getEngine()->scheduleParameterBang(this->paramInitIndices[i], 0);
    }
}

void updateTime(MillisecondTime time, EXTERNALENGINE* engine, bool inProcess = false) {
    RNBO_UNUSED(inProcess);
    RNBO_UNUSED(engine);
    this->_currentTime = time;
    auto offset = rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr));

    if (offset >= (SampleIndex)(this->vs))
        offset = (SampleIndex)(this->vs) - 1;

    if (offset < 0)
        offset = 0;

    this->sampleOffsetIntoNextAudioBuffer = (Index)(offset);
}

void assign_defaults()
{
    data_01_sizeout = 0;
    data_01_size = 1;
    data_01_sizems = 0;
    data_01_normalize = 0.995;
    data_01_channels = 2;
    data_02_sizeout = 0;
    data_02_size = 0;
    data_02_sizems = 0;
    data_02_normalize = 0.995;
    data_02_channels = 2;
    param_01_value = 50;
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_movDur = 0;
    gen_01_grainSize = 0;
    gen_01_reverse = 0;
    gen_01_Mix = 0;
    gen_01_delTime = 0;
    gen_01_feedCoe = 0;
    gen_01_density = 0;
    param_02_value = 50;
    param_03_value = 500;
    param_04_value = 1000;
    param_05_value = 0.5;
    param_06_value = 100;
    param_07_value = 0;
    ctlin_01_input = 0;
    ctlin_01_controller = 0;
    ctlin_01_channel = -1;
    expr_01_in1 = 0;
    expr_01_in2 = 0.007874015748;
    expr_01_out1 = 0;
    ctlin_02_input = 0;
    ctlin_02_controller = 0;
    ctlin_02_channel = -1;
    expr_02_in1 = 0;
    expr_02_in2 = 0.007874015748;
    expr_02_out1 = 0;
    ctlin_03_input = 0;
    ctlin_03_controller = 0;
    ctlin_03_channel = -1;
    expr_03_in1 = 0;
    expr_03_in2 = 0.007874015748;
    expr_03_out1 = 0;
    ctlin_04_input = 0;
    ctlin_04_controller = 0;
    ctlin_04_channel = -1;
    expr_04_in1 = 0;
    expr_04_in2 = 0.007874015748;
    expr_04_out1 = 0;
    ctlin_05_input = 0;
    ctlin_05_controller = 0;
    ctlin_05_channel = -1;
    expr_05_in1 = 0;
    expr_05_in2 = 0.007874015748;
    expr_05_out1 = 0;
    ctlin_06_input = 0;
    ctlin_06_controller = 0;
    ctlin_06_channel = -1;
    expr_06_in1 = 0;
    expr_06_in2 = 0.007874015748;
    expr_06_out1 = 0;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.000022675736961451248;
    data_01_sizemode = 1;
    data_01_setupDone = false;
    data_02_sizemode = 3;
    data_02_setupDone = false;
    param_01_lastValue = 0;
    gen_01_lastPhaseReader1_value = 0;
    gen_01_lastPhaseReader2_value = 0;
    gen_01_PhaseDiff1_value = 0;
    gen_01_PhaseDiff2_value = 0;
    gen_01_delL__maxdelay = 0;
    gen_01_delL_sizemode = 0;
    gen_01_delL_wrap = 0;
    gen_01_delL_reader = 0;
    gen_01_delL_writer = 0;
    gen_01_delR__maxdelay = 0;
    gen_01_delR_sizemode = 0;
    gen_01_delR_wrap = 0;
    gen_01_delR_reader = 0;
    gen_01_delR_writer = 0;
    gen_01_counter_1_carry = 0;
    gen_01_counter_1_count = 0;
    gen_01_counter_3_carry = 0;
    gen_01_counter_3_count = 0;
    gen_01_latch_5_value = 0;
    gen_01_latch_7_value = 0;
    gen_01_sah_9_prev = 0;
    gen_01_sah_9_value = 0;
    gen_01_sah_12_prev = 0;
    gen_01_sah_12_value = 0;
    gen_01_phasor_16_currentPhase = 0;
    gen_01_phasor_16_conv = 0;
    gen_01_phasor_18_currentPhase = 0;
    gen_01_phasor_18_conv = 0;
    gen_01_latch_22_value = 0;
    gen_01_latch_26_value = 0;
    gen_01_dcblock_42_xm1 = 0;
    gen_01_dcblock_42_ym1 = 0;
    gen_01_dcblock_44_xm1 = 0;
    gen_01_dcblock_44_ym1 = 0;
    gen_01_setupDone = false;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    param_05_lastValue = 0;
    param_06_lastValue = 0;
    param_07_lastValue = 0;
    ctlin_01_status = 0;
    ctlin_01_byte1 = -1;
    ctlin_01_inchan = 0;
    ctlin_02_status = 0;
    ctlin_02_byte1 = -1;
    ctlin_02_inchan = 0;
    ctlin_03_status = 0;
    ctlin_03_byte1 = -1;
    ctlin_03_inchan = 0;
    ctlin_04_status = 0;
    ctlin_04_byte1 = -1;
    ctlin_04_inchan = 0;
    ctlin_05_status = 0;
    ctlin_05_byte1 = -1;
    ctlin_05_inchan = 0;
    ctlin_06_status = 0;
    ctlin_06_byte1 = -1;
    ctlin_06_inchan = 0;
    globaltransport_tempo = nullptr;
    globaltransport_state = nullptr;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

    // data ref strings
    struct DataRefStrings {
    	static constexpr auto& name0 = "ringBuffIndex";
    	static constexpr auto& file0 = "";
    	static constexpr auto& tag0 = "buffer~";
    	static constexpr auto& name1 = "ringBuff";
    	static constexpr auto& file1 = "";
    	static constexpr auto& tag1 = "buffer~";
    	static constexpr auto& name2 = "gen_01_delL_bufferobj";
    	static constexpr auto& file2 = "";
    	static constexpr auto& tag2 = "buffer~";
    	static constexpr auto& name3 = "gen_01_delR_bufferobj";
    	static constexpr auto& file3 = "";
    	static constexpr auto& tag3 = "buffer~";
    	DataRefStrings* operator->() { return this; }
    	const DataRefStrings* operator->() const { return this; }
    };

    DataRefStrings dataRefStrings;

// member variables

    number data_01_sizeout;
    number data_01_size;
    number data_01_sizems;
    number data_01_normalize;
    number data_01_channels;
    number data_02_sizeout;
    number data_02_size;
    number data_02_sizems;
    number data_02_normalize;
    number data_02_channels;
    number param_01_value;
    number gen_01_in1;
    number gen_01_in2;
    number gen_01_movDur;
    number gen_01_grainSize;
    number gen_01_reverse;
    number gen_01_Mix;
    number gen_01_delTime;
    number gen_01_feedCoe;
    number gen_01_density;
    number param_02_value;
    number param_03_value;
    number param_04_value;
    number param_05_value;
    number param_06_value;
    number param_07_value;
    number ctlin_01_input;
    number ctlin_01_controller;
    number ctlin_01_channel;
    number expr_01_in1;
    number expr_01_in2;
    number expr_01_out1;
    number ctlin_02_input;
    number ctlin_02_controller;
    number ctlin_02_channel;
    number expr_02_in1;
    number expr_02_in2;
    number expr_02_out1;
    number ctlin_03_input;
    number ctlin_03_controller;
    number ctlin_03_channel;
    number expr_03_in1;
    number expr_03_in2;
    number expr_03_out1;
    number ctlin_04_input;
    number ctlin_04_controller;
    number ctlin_04_channel;
    number expr_04_in1;
    number expr_04_in2;
    number expr_04_out1;
    number ctlin_05_input;
    number ctlin_05_controller;
    number ctlin_05_channel;
    number expr_05_in1;
    number expr_05_in2;
    number expr_05_out1;
    number ctlin_06_input;
    number ctlin_06_controller;
    number ctlin_06_channel;
    number expr_06_in1;
    number expr_06_in2;
    number expr_06_out1;
    MillisecondTime _currentTime;
    ENGINE _internalEngine;
    UInt64 audioProcessSampleCount;
    Index sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    Float32BufferRef data_01_buffer;
    Int data_01_sizemode;
    bool data_01_setupDone;
    Float32BufferRef data_02_buffer;
    Int data_02_sizemode;
    bool data_02_setupDone;
    number param_01_lastValue;
    Float32BufferRef gen_01_RingBuff;
    Float32BufferRef gen_01_RingBuffIndex;
    number gen_01_lastPhaseReader1_value;
    number gen_01_lastPhaseReader2_value;
    number gen_01_PhaseDiff1_value;
    number gen_01_PhaseDiff2_value;
    Float64BufferRef gen_01_delL_buffer;
    Index gen_01_delL__maxdelay;
    Int gen_01_delL_sizemode;
    Index gen_01_delL_wrap;
    Int gen_01_delL_reader;
    Int gen_01_delL_writer;
    Float64BufferRef gen_01_delR_buffer;
    Index gen_01_delR__maxdelay;
    Int gen_01_delR_sizemode;
    Index gen_01_delR_wrap;
    Int gen_01_delR_reader;
    Int gen_01_delR_writer;
    Int gen_01_counter_1_carry;
    number gen_01_counter_1_count;
    Int gen_01_counter_3_carry;
    number gen_01_counter_3_count;
    number gen_01_latch_5_value;
    number gen_01_latch_7_value;
    number gen_01_sah_9_prev;
    number gen_01_sah_9_value;
    UInt gen_01_noise_10_state[4] = { };
    number gen_01_sah_12_prev;
    number gen_01_sah_12_value;
    UInt gen_01_noise_13_state[4] = { };
    number gen_01_phasor_16_currentPhase;
    number gen_01_phasor_16_conv;
    number gen_01_phasor_18_currentPhase;
    number gen_01_phasor_18_conv;
    UInt gen_01_noise_20_state[4] = { };
    number gen_01_latch_22_value;
    UInt gen_01_noise_24_state[4] = { };
    number gen_01_latch_26_value;
    number gen_01_dcblock_42_xm1;
    number gen_01_dcblock_42_ym1;
    number gen_01_dcblock_44_xm1;
    number gen_01_dcblock_44_ym1;
    bool gen_01_setupDone;
    number param_02_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    number param_05_lastValue;
    number param_06_lastValue;
    number param_07_lastValue;
    Int ctlin_01_status;
    Int ctlin_01_byte1;
    Int ctlin_01_inchan;
    Int ctlin_02_status;
    Int ctlin_02_byte1;
    Int ctlin_02_inchan;
    Int ctlin_03_status;
    Int ctlin_03_byte1;
    Int ctlin_03_inchan;
    Int ctlin_04_status;
    Int ctlin_04_byte1;
    Int ctlin_04_inchan;
    Int ctlin_05_status;
    Int ctlin_05_byte1;
    Int ctlin_05_inchan;
    Int ctlin_06_status;
    Int ctlin_06_byte1;
    Int ctlin_06_inchan;
    signal globaltransport_tempo;
    signal globaltransport_state;
    number stackprotect_count;
    DataRef ringBuffIndex;
    DataRef ringBuff;
    DataRef gen_01_delL_bufferobj;
    DataRef gen_01_delR_bufferobj;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;
    bool _isInitialized = false;
};

static PatcherInterface* creaternbomatic()
{
    return new rnbomatic<EXTERNALENGINE>();
}

#ifndef RNBO_NO_PATCHERFACTORY
extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction()
#else
extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction()
#endif
{
    return creaternbomatic;
}

#ifndef RNBO_NO_PATCHERFACTORY
extern "C" void SetLogger(Logger* logger)
#else
void rnbomaticSetLogger(Logger* logger)
#endif
{
    console = logger;
}

} // end RNBO namespace

