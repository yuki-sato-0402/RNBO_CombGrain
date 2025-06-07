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

#include "RNBO_Common.h"
#include "RNBO_AudioSignal.h"

namespace RNBO {


#define trunc(x) ((Int)(x))

#if defined(__GNUC__) || defined(__clang__)
    #define RNBO_RESTRICT __restrict__
#elif defined(_MSC_VER)
    #define RNBO_RESTRICT __restrict
#endif

#define FIXEDSIZEARRAYINIT(...) { }

class rnbomatic : public PatcherInterfaceImpl {
public:

rnbomatic()
{
}

~rnbomatic()
{
}

rnbomatic* getTopLevelPatcher() {
    return this;
}

void cancelClockEvents()
{
}

template <typename T> void listquicksort(T& arr, T& sortindices, Int l, Int h, bool ascending) {
    if (l < h) {
        Int p = (Int)(this->listpartition(arr, sortindices, l, h, ascending));
        this->listquicksort(arr, sortindices, l, p - 1, ascending);
        this->listquicksort(arr, sortindices, p + 1, h, ascending);
    }
}

template <typename T> Int listpartition(T& arr, T& sortindices, Int l, Int h, bool ascending) {
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

template <typename T> void listswapelements(T& arr, Int a, Int b) {
    auto tmp = arr[(Index)a];
    arr[(Index)a] = arr[(Index)b];
    arr[(Index)b] = tmp;
}

inline number linearinterp(number frac, number x, number y) {
    return x + (y - x) * frac;
}

inline number cubicinterp(number a, number w, number x, number y, number z) {
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

number mstosamps(MillisecondTime ms) {
    return ms * this->sr * 0.001;
}

number samplerate() {
    return this->sr;
}

Index vectorsize() {
    return this->vs;
}

number maximum(number x, number y) {
    return (x < y ? y : x);
}

Index voice() {
    return this->_voiceIndex;
}

number random(number low, number high) {
    number range = high - low;
    return rand01() * range + low;
}

template <typename T> inline number dim(T& buffer) {
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

    long numWraps = (long)(trunc((x - lo) / range));
    numWraps = numWraps - ((x < lo ? 1 : 0));
    number result = x - range * numWraps;

    if (result >= hi)
        return result - range;
    else
        return result;
}

template <typename T> inline void poke_boundmode_wrap(
    T& buffer,
    SampleValue value,
    SampleValue sampleIndex,
    int channel,
    number overdub
) {
    number bufferSize = buffer->getSize();
    const Index bufferChannels = (const Index)(buffer->getChannels());

    if (bufferSize > 0 && (3 != 5 || (sampleIndex >= 0 && sampleIndex < bufferSize)) && (5 != 5 || (channel >= 0 && channel < bufferChannels))) {
        {
            SampleIndex bufferbindindex_result;

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
                value = value * (1. - overdub) + currentValue * overdub;
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

template <typename T> inline array<SampleValue, 1 + 1> wave_interp_cubic(
    T& buffer,
    SampleValue phase,
    SampleValue start,
    SampleValue end,
    int channelOffset
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
            auto __end = end + 1;
            auto __start = start;
            auto __phase = phase;

            {
                {
                    {
                        number size = __end - 1 - __start;
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
            SampleIndex bufferbindindex_result;

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

                SampleIndex index1 = (SampleIndex)(trunc(sampleIndex));

                {
                    {
                        number frac = sampleIndex - index1;
                        number wrap = end - 1;
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

            out[(Index)channel] = bufferreadsample_result;
        }

        out[1] = sampleIndex - start;
        return out;
    }
}

number __wrapped_op_clamp(number in1, number in2, number in3) {
    return (in1 > in3 ? in3 : (in1 < in2 ? in2 : in1));
}

MillisecondTime currenttime() {
    return this->_currentTime;
}

number tempo() {
    return this->getTopLevelPatcher()->globaltransport_getTempo(this->currenttime());
}

number mstobeats(number ms) {
    return ms * this->tempo() * 0.008 / (number)480;
}

MillisecondTime sampstoms(number samps) {
    return samps * 1000 / this->sr;
}

Index getNumMidiInputPorts() const {
    return 0;
}

void processMidiEvent(MillisecondTime , int , ConstByteArray , Index ) {}

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
    this->updateTime(this->getEngine()->getCurrentTime());
    SampleValue * out1 = (numOutputs >= 1 && outputs[0] ? outputs[0] : this->dummyBuffer);
    SampleValue * out2 = (numOutputs >= 2 && outputs[1] ? outputs[1] : this->dummyBuffer);
    const SampleValue * in1 = (numInputs >= 1 && inputs[0] ? inputs[0] : this->zeroBuffer);
    const SampleValue * in2 = (numInputs >= 2 && inputs[1] ? inputs[1] : this->zeroBuffer);

    this->gen_01_perform(
        in1,
        in2,
        this->gen_01_FeedCoe,
        this->gen_01_delTime,
        this->gen_01_Mix,
        this->gen_01_reverse,
        this->gen_01_grainSpeed,
        this->gen_01_movDur,
        out1,
        out2,
        n
    );

    this->stackprotect_perform(n);
    this->globaltransport_advance();
    this->audioProcessSampleCount += this->vs;
}

void prepareToProcess(number sampleRate, Index maxBlockSize, bool force) {
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

    this->gen_01_dspsetup(forceDSPSetup);
    this->globaltransport_dspsetup(forceDSPSetup);

    if (sampleRateChanged)
        this->onSampleRateChanged(sampleRate);
}

void setProbingTarget(MessageTag id) {
    switch (id) {
    default:
        {
        this->setProbingIndex(-1);
        break;
        }
    }
}

void setProbingIndex(ProbingIndex ) {}

Index getProbingChannels(MessageTag outletId) const {
    RNBO_UNUSED(outletId);
    return 0;
}

DataRef* getDataRef(DataRefIndex index)  {
    switch (index) {
    case 0:
        {
        return addressOf(this->RingBuff);
        break;
        }
    case 1:
        {
        return addressOf(this->gen_01_delR_bufferobj);
        break;
        }
    case 2:
        {
        return addressOf(this->gen_01_delL_bufferobj);
        break;
        }
    default:
        {
        return nullptr;
        }
    }
}

DataRefIndex getNumDataRefs() const {
    return 3;
}

void fillDataRef(DataRefIndex , DataRef& ) {}

void zeroDataRef(DataRef& ref) {
    ref->setZero();
}

void processDataViewUpdate(DataRefIndex index, MillisecondTime time) {
    this->updateTime(time);

    if (index == 0) {
        this->gen_01_RingBuff = new Float64Buffer(this->RingBuff);
        this->gen_01_RingBuff_exprdata_buffer = new Float64Buffer(this->RingBuff);
    }

    if (index == 1) {
        this->gen_01_delR_buffer = new Float64Buffer(this->gen_01_delR_bufferobj);
    }

    if (index == 2) {
        this->gen_01_delL_buffer = new Float64Buffer(this->gen_01_delL_bufferobj);
    }
}

void initialize() {
    this->RingBuff = initDataRef("RingBuff", true, nullptr, "buffer~");
    this->gen_01_delR_bufferobj = initDataRef("gen_01_delR_bufferobj", true, nullptr, "buffer~");
    this->gen_01_delL_bufferobj = initDataRef("gen_01_delL_bufferobj", true, nullptr, "buffer~");
    this->assign_defaults();
    this->setState();
    this->RingBuff->setIndex(0);
    this->gen_01_RingBuff = new Float64Buffer(this->RingBuff);
    this->gen_01_RingBuff_exprdata_buffer = new Float64Buffer(this->RingBuff);
    this->gen_01_delR_bufferobj->setIndex(1);
    this->gen_01_delR_buffer = new Float64Buffer(this->gen_01_delR_bufferobj);
    this->gen_01_delL_bufferobj->setIndex(2);
    this->gen_01_delL_buffer = new Float64Buffer(this->gen_01_delL_bufferobj);
    this->initializeObjects();
    this->allocateDataRefs();
    this->startup();
}

Index getIsMuted()  {
    return this->isMuted;
}

void setIsMuted(Index v)  {
    this->isMuted = v;
}

Index getPatcherSerial() const {
    return 0;
}

void getState(PatcherStateInterface& ) {}

void setState() {}

void getPreset(PatcherStateInterface& preset) {
    preset["__presetid"] = "rnbo";
    this->param_01_getPresetValue(getSubState(preset, "Mix"));
    this->param_02_getPresetValue(getSubState(preset, "movDur"));
    this->param_03_getPresetValue(getSubState(preset, "grainSpeed"));
    this->param_04_getPresetValue(getSubState(preset, "FeedCoe"));
    this->param_05_getPresetValue(getSubState(preset, "delTime"));
    this->param_06_getPresetValue(getSubState(preset, "reverse"));
}

void setPreset(MillisecondTime time, PatcherStateInterface& preset) {
    this->updateTime(time);
    this->param_01_setPresetValue(getSubState(preset, "Mix"));
    this->param_02_setPresetValue(getSubState(preset, "movDur"));
    this->param_03_setPresetValue(getSubState(preset, "grainSpeed"));
    this->param_04_setPresetValue(getSubState(preset, "FeedCoe"));
    this->param_05_setPresetValue(getSubState(preset, "delTime"));
    this->param_06_setPresetValue(getSubState(preset, "reverse"));
}

void processTempoEvent(MillisecondTime time, Tempo tempo) {
    this->updateTime(time);

    if (this->globaltransport_setTempo(this->_currentTime, tempo, false))
        {}
}

void processTransportEvent(MillisecondTime time, TransportState state) {
    this->updateTime(time);

    if (this->globaltransport_setState(this->_currentTime, state, false))
        {}
}

void processBeatTimeEvent(MillisecondTime time, BeatTime beattime) {
    this->updateTime(time);

    if (this->globaltransport_setBeatTime(this->_currentTime, beattime, false))
        {}
}

void onSampleRateChanged(double ) {}

void processTimeSignatureEvent(MillisecondTime time, int numerator, int denominator) {
    this->updateTime(time);

    if (this->globaltransport_setTimeSignature(this->_currentTime, numerator, denominator, false))
        {}
}

void setParameterValue(ParameterIndex index, ParameterValue v, MillisecondTime time) {
    this->updateTime(time);

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
    return 6;
}

ConstCharPointer getParameterName(ParameterIndex index) const {
    switch (index) {
    case 0:
        {
        return "Mix";
        }
    case 1:
        {
        return "movDur";
        }
    case 2:
        {
        return "grainSpeed";
        }
    case 3:
        {
        return "FeedCoe";
        }
    case 4:
        {
        return "delTime";
        }
    case 5:
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
        return "Mix";
        }
    case 1:
        {
        return "movDur";
        }
    case 2:
        {
        return "grainSpeed";
        }
    case 3:
        {
        return "FeedCoe";
        }
    case 4:
        {
        return "delTime";
        }
    case 5:
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
        case 1:
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
        case 2:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 1;
            info->min = 0.2;
            info->max = 30;
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
        case 4:
            {
            info->type = ParameterTypeNumber;
            info->initialValue = 100;
            info->min = 2;
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
        case 5:
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

void sendParameter(ParameterIndex index, bool ignoreValue) {
    this->getEngine()->notifyParameterValueChanged(index, (ignoreValue ? 0 : this->getParameterValue(index)), ignoreValue);
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
    case 5:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));
            ParameterValue normalizedValue = (value - 0) / (1 - 0);
            return normalizedValue;
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 100 ? 100 : value));
            ParameterValue normalizedValue = (value - 0) / (100 - 0);
            return normalizedValue;
        }
        }
    case 4:
        {
        {
            value = (value < 2 ? 2 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 2) / (1000 - 2);
            return normalizedValue;
        }
        }
    case 1:
        {
        {
            value = (value < 5 ? 5 : (value > 1000 ? 1000 : value));
            ParameterValue normalizedValue = (value - 5) / (1000 - 5);
            return normalizedValue;
        }
        }
    case 2:
        {
        {
            value = (value < 0.2 ? 0.2 : (value > 30 ? 30 : value));
            ParameterValue normalizedValue = (value - 0.2) / (30 - 0.2);
            return normalizedValue;
        }
        }
    case 3:
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
    case 5:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (1 - 0);
            }
        }
        }
    case 0:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0 + value * (100 - 0);
            }
        }
        }
    case 4:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 2 + value * (1000 - 2);
            }
        }
        }
    case 1:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 5 + value * (1000 - 5);
            }
        }
        }
    case 2:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

            {
                return 0.2 + value * (30 - 0.2);
            }
        }
        }
    case 3:
        {
        {
            value = (value < 0 ? 0 : (value > 1 ? 1 : value));

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
    default:
        {
        return value;
        }
    }
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

void processClockEvent(MillisecondTime , ClockId , bool , ParameterValue ) {}

void processOutletAtCurrentTime(EngineLink* , OutletIndex , ParameterValue ) {}

void processOutletEvent(
    EngineLink* sender,
    OutletIndex index,
    ParameterValue value,
    MillisecondTime time
) {
    this->updateTime(time);
    this->processOutletAtCurrentTime(sender, index, value);
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

void param_01_value_set(number v) {
    v = this->param_01_value_constrain(v);
    this->param_01_value = v;
    this->sendParameter(0, false);

    if (this->param_01_value != this->param_01_lastValue) {
        this->getEngine()->presetTouched();
        this->param_01_lastValue = this->param_01_value;
    }

    this->gen_01_Mix_set(v);
}

void param_02_value_set(number v) {
    v = this->param_02_value_constrain(v);
    this->param_02_value = v;
    this->sendParameter(1, false);

    if (this->param_02_value != this->param_02_lastValue) {
        this->getEngine()->presetTouched();
        this->param_02_lastValue = this->param_02_value;
    }

    this->gen_01_movDur_set(v);
}

void param_03_value_set(number v) {
    v = this->param_03_value_constrain(v);
    this->param_03_value = v;
    this->sendParameter(2, false);

    if (this->param_03_value != this->param_03_lastValue) {
        this->getEngine()->presetTouched();
        this->param_03_lastValue = this->param_03_value;
    }

    this->gen_01_grainSpeed_set(v);
}

void param_04_value_set(number v) {
    v = this->param_04_value_constrain(v);
    this->param_04_value = v;
    this->sendParameter(3, false);

    if (this->param_04_value != this->param_04_lastValue) {
        this->getEngine()->presetTouched();
        this->param_04_lastValue = this->param_04_value;
    }

    this->gen_01_FeedCoe_set(v);
}

void param_05_value_set(number v) {
    v = this->param_05_value_constrain(v);
    this->param_05_value = v;
    this->sendParameter(4, false);

    if (this->param_05_value != this->param_05_lastValue) {
        this->getEngine()->presetTouched();
        this->param_05_lastValue = this->param_05_value;
    }

    this->gen_01_delTime_set(v);
}

void param_06_value_set(number v) {
    v = this->param_06_value_constrain(v);
    this->param_06_value = v;
    this->sendParameter(5, false);

    if (this->param_06_value != this->param_06_lastValue) {
        this->getEngine()->presetTouched();
        this->param_06_lastValue = this->param_06_value;
    }

    this->gen_01_reverse_set(v);
}

number msToSamps(MillisecondTime ms, number sampleRate) {
    return ms * sampleRate * 0.001;
}

MillisecondTime sampsToMs(SampleIndex samps) {
    return samps * (this->invsr * 1000);
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

Index getNumInputChannels() const {
    return 2;
}

Index getNumOutputChannels() const {
    return 2;
}

void allocateDataRefs() {
    this->gen_01_RingBuff = this->gen_01_RingBuff->allocateIfNeeded();
    this->gen_01_RingBuff_exprdata_buffer = this->gen_01_RingBuff_exprdata_buffer->allocateIfNeeded();

    if (this->RingBuff->hasRequestedSize()) {
        if (this->RingBuff->wantsFill())
            this->zeroDataRef(this->RingBuff);

        this->getEngine()->sendDataRefUpdated(0);
    }

    this->gen_01_delR_buffer = this->gen_01_delR_buffer->allocateIfNeeded();

    if (this->gen_01_delR_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delR_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delR_bufferobj);

        this->getEngine()->sendDataRefUpdated(1);
    }

    this->gen_01_delL_buffer = this->gen_01_delL_buffer->allocateIfNeeded();

    if (this->gen_01_delL_bufferobj->hasRequestedSize()) {
        if (this->gen_01_delL_bufferobj->wantsFill())
            this->zeroDataRef(this->gen_01_delL_bufferobj);

        this->getEngine()->sendDataRefUpdated(2);
    }
}

void initializeObjects() {
    this->gen_01_delR_init();
    this->gen_01_delL_init();
    this->gen_01_PhaseDiff2_init();
    this->gen_01_PhaseDiff1_init();
    this->gen_01_lastPhaseReader2_init();
    this->gen_01_lastPhaseReader1_init();
    this->gen_01_RingBuff_exprdata_init();
    this->gen_01_counter_1_init();
    this->gen_01_counter_3_init();
    this->gen_01_noise_10_init();
    this->gen_01_noise_13_init();
}

void sendOutlet(OutletIndex index, ParameterValue value) {
    this->getEngine()->sendOutlet(this, index, value);
}

void startup() {
    this->updateTime(this->getEngine()->getCurrentTime());

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

    this->processParamInitEvents();
}

static number param_01_value_constrain(number v) {
    v = (v > 100 ? 100 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_Mix_set(number v) {
    this->gen_01_Mix = v;
}

static number param_02_value_constrain(number v) {
    v = (v > 1000 ? 1000 : (v < 5 ? 5 : v));
    return v;
}

void gen_01_movDur_set(number v) {
    this->gen_01_movDur = v;
}

static number param_03_value_constrain(number v) {
    v = (v > 30 ? 30 : (v < 0.2 ? 0.2 : v));
    return v;
}

void gen_01_grainSpeed_set(number v) {
    this->gen_01_grainSpeed = v;
}

static number param_04_value_constrain(number v) {
    v = (v > 0.9 ? 0.9 : (v < 0.01 ? 0.01 : v));
    return v;
}

void gen_01_FeedCoe_set(number v) {
    this->gen_01_FeedCoe = v;
}

static number param_05_value_constrain(number v) {
    v = (v > 1000 ? 1000 : (v < 2 ? 2 : v));
    return v;
}

void gen_01_delTime_set(number v) {
    this->gen_01_delTime = v;
}

static number param_06_value_constrain(number v) {
    v = (v > 1 ? 1 : (v < 0 ? 0 : v));
    return v;
}

void gen_01_reverse_set(number v) {
    this->gen_01_reverse = v;
}

void gen_01_perform(
    const Sample * in1,
    const Sample * in2,
    number FeedCoe,
    number delTime,
    number Mix,
    number reverse,
    number grainSpeed,
    number movDur,
    SampleValue * out1,
    SampleValue * out2,
    Index n
) {
    auto __gen_01_lastPhaseReader2_value = this->gen_01_lastPhaseReader2_value;
    auto __gen_01_lastPhaseReader1_value = this->gen_01_lastPhaseReader1_value;
    auto __gen_01_PhaseDiff2_value = this->gen_01_PhaseDiff2_value;
    auto __gen_01_PhaseDiff1_value = this->gen_01_PhaseDiff1_value;
    auto grainSize_0 = this->mstosamps((grainSpeed == 0. ? 0. : (number)1000 / grainSpeed));
    Index i;

    for (i = 0; i < n; i++) {
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
        number holdGrainSize1_6 = this->gen_01_latch_5_next(grainSize_0, __gen_01_PhaseDiff1_value >= 0.99);
        number holdGrainSize2_8 = this->gen_01_latch_7_next(grainSize_0, __gen_01_PhaseDiff2_value >= 0.99);

        auto pos1_11 = this->scale(
            this->gen_01_sah_9_next(this->gen_01_noise_10_next(), MovCount1, 0),
            -1,
            1,
            0,
            this->dim(this->gen_01_RingBuff) - 2 * holdGrainSize1_6,
            1
        );

        auto pos2_14 = this->scale(
            this->gen_01_sah_12_next(this->gen_01_noise_13_next(), MovCount1, 0),
            -1,
            1,
            0,
            this->dim(this->gen_01_RingBuff) - 2 * holdGrainSize2_8,
            1
        );

        number hann_window1_15 = 0.5 - 0.5 * rnbo_cos(
            6.28318530717958647692 * ((this->mstosamps(movDur) == 0. ? 0. : MovCount1 / this->mstosamps(movDur)))
        );

        number phaseReader1_17 = this->gen_01_phasor_16_next(grainSpeed, 0);
        auto phaseReader2_19 = this->wrap(this->gen_01_phasor_18_next(grainSpeed, 0) + 0.5, 0, 1);
        __gen_01_PhaseDiff1_value = __gen_01_lastPhaseReader1_value - phaseReader1_17;
        __gen_01_PhaseDiff2_value = __gen_01_lastPhaseReader2_value - phaseReader2_19;
        __gen_01_lastPhaseReader1_value = phaseReader1_17;
        __gen_01_lastPhaseReader2_value = phaseReader2_19;
        number hann_window2_20 = 0.5 - 0.5 * rnbo_cos(6.28318530717958647692 * phaseReader1_17);
        number hann_window3_21 = 0.5 - 0.5 * rnbo_cos(6.28318530717958647692 * phaseReader2_19);

        if (reverse == 1) {
            phaseReader1_17 = 1 - phaseReader1_17;
        }

        number grainL = 0;
        number waveIndex1 = 0;

        auto result_22 = this->wave_interp_cubic(
            this->gen_01_RingBuff,
            phaseReader1_17,
            pos1_11,
            pos1_11 + holdGrainSize1_6,
            0
        );

        waveIndex1 = result_22[1];
        grainL = result_22[0];
        number grainR = 0;
        number waveIndex2 = 0;

        auto result_23 = this->wave_interp_cubic(
            this->gen_01_RingBuff,
            phaseReader2_19,
            pos2_14,
            pos2_14 + holdGrainSize2_8,
            1
        );

        waveIndex2 = result_23[1];
        grainR = result_23[0];
        number windowGrainL_24 = grainL * hann_window1_15 * hann_window2_20;
        number windowGrainR_25 = grainR * hann_window1_15 * hann_window3_21;
        number delGrainL_26 = this->gen_01_delL_read(delTime, 1);
        number feedGrainL_27 = delGrainL_26 * FeedCoe + windowGrainL_24;
        this->gen_01_delL_write(windowGrainL_24 + feedGrainL_27);
        number delGrainR_28 = this->gen_01_delR_read(delTime, 1);
        number feedGrainR_29 = delGrainR_28 * FeedCoe + windowGrainR_25;
        this->gen_01_delR_write(windowGrainR_25 + feedGrainR_29);

        number expr_1_31 = this->__wrapped_op_clamp(
            in1[(Index)i] + Mix * 0.01 * (this->gen_01_dcblock_30_next(feedGrainL_27 * 0.8, 0.9997) - in1[(Index)i]),
            -1,
            1
        );

        number expr_2_33 = this->__wrapped_op_clamp(
            in2[(Index)i] + Mix * 0.01 * (this->gen_01_dcblock_32_next(feedGrainR_29 * 0.8, 0.9997) - in2[(Index)i]),
            -1,
            1
        );

        out2[(Index)i] = expr_2_33;
        out1[(Index)i] = expr_1_31;
        this->gen_01_delR_step();
        this->gen_01_delL_step();
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

void gen_01_delR_step() {
    this->gen_01_delR_reader++;

    if (this->gen_01_delR_reader >= (int)(this->gen_01_delR_buffer->getSize()))
        this->gen_01_delR_reader = 0;
}

number gen_01_delR_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        {
            number r = (int)(this->gen_01_delR_buffer->getSize()) + this->gen_01_delR_reader - ((size > this->gen_01_delR__maxdelay ? this->gen_01_delR__maxdelay : (size < (1 + this->gen_01_delR_reader != this->gen_01_delR_writer) ? 1 + this->gen_01_delR_reader != this->gen_01_delR_writer : size)));
            long index1 = (long)(rnbo_floor(r));
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

    number r = (int)(this->gen_01_delR_buffer->getSize()) + this->gen_01_delR_reader - ((size > this->gen_01_delR__maxdelay ? this->gen_01_delR__maxdelay : (size < (this->gen_01_delR_reader != this->gen_01_delR_writer) ? this->gen_01_delR_reader != this->gen_01_delR_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delR_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delR_wrap))
    );
}

void gen_01_delR_write(number v) {
    this->gen_01_delR_writer = this->gen_01_delR_reader;
    this->gen_01_delR_buffer[(Index)this->gen_01_delR_writer] = v;
}

number gen_01_delR_next(number v, int size) {
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
        sizeInSamples = this->gen_01_delR_evaluateSizeExpr(this->samplerate(), this->vectorsize());
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
    return this->samplerate();
}

number gen_01_delR_size() {
    return this->gen_01_delR__maxdelay;
}

void gen_01_delL_step() {
    this->gen_01_delL_reader++;

    if (this->gen_01_delL_reader >= (int)(this->gen_01_delL_buffer->getSize()))
        this->gen_01_delL_reader = 0;
}

number gen_01_delL_read(number size, Int interp) {
    RNBO_UNUSED(interp);

    {
        {
            number r = (int)(this->gen_01_delL_buffer->getSize()) + this->gen_01_delL_reader - ((size > this->gen_01_delL__maxdelay ? this->gen_01_delL__maxdelay : (size < (1 + this->gen_01_delL_reader != this->gen_01_delL_writer) ? 1 + this->gen_01_delL_reader != this->gen_01_delL_writer : size)));
            long index1 = (long)(rnbo_floor(r));
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

    number r = (int)(this->gen_01_delL_buffer->getSize()) + this->gen_01_delL_reader - ((size > this->gen_01_delL__maxdelay ? this->gen_01_delL__maxdelay : (size < (this->gen_01_delL_reader != this->gen_01_delL_writer) ? this->gen_01_delL_reader != this->gen_01_delL_writer : size)));
    long index1 = (long)(rnbo_floor(r));

    return this->gen_01_delL_buffer->getSample(
        0,
        (Index)((BinOpInt)((BinOpInt)index1 & (BinOpInt)this->gen_01_delL_wrap))
    );
}

void gen_01_delL_write(number v) {
    this->gen_01_delL_writer = this->gen_01_delL_reader;
    this->gen_01_delL_buffer[(Index)this->gen_01_delL_writer] = v;
}

number gen_01_delL_next(number v, int size) {
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
        sizeInSamples = this->gen_01_delL_evaluateSizeExpr(this->samplerate(), this->vectorsize());
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
    return this->samplerate();
}

number gen_01_delL_size() {
    return this->gen_01_delL__maxdelay;
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

void gen_01_RingBuff_exprdata_init() {
    number sizeInSamples = this->gen_01_RingBuff_exprdata_evaluateSizeExpr(this->samplerate(), this->vectorsize());
    this->gen_01_RingBuff_exprdata_buffer->requestSize(sizeInSamples, 2);
}

void gen_01_RingBuff_exprdata_dspsetup() {
    number sizeInSamples = this->gen_01_RingBuff_exprdata_evaluateSizeExpr(this->samplerate(), this->vectorsize());
    this->gen_01_RingBuff_exprdata_buffer = this->gen_01_RingBuff_exprdata_buffer->setSize(sizeInSamples);
    updateDataRef(this, this->gen_01_RingBuff_exprdata_buffer);
}

number gen_01_RingBuff_exprdata_evaluateSizeExpr(number samplerate, number vectorsize) {
    RNBO_UNUSED(vectorsize);
    return samplerate * 2;
}

void gen_01_RingBuff_exprdata_reset() {}

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
    if (control != 0.)
        this->gen_01_latch_5_value = x;

    return this->gen_01_latch_5_value;
}

void gen_01_latch_5_dspsetup() {
    this->gen_01_latch_5_reset();
}

void gen_01_latch_5_reset() {
    this->gen_01_latch_5_value = 0;
}

number gen_01_latch_7_next(number x, number control) {
    if (control != 0.)
        this->gen_01_latch_7_value = x;

    return this->gen_01_latch_7_value;
}

void gen_01_latch_7_dspsetup() {
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

number gen_01_dcblock_30_next(number x, number gain) {
    RNBO_UNUSED(gain);
    number y = x - this->gen_01_dcblock_30_xm1 + this->gen_01_dcblock_30_ym1 * 0.9997;
    this->gen_01_dcblock_30_xm1 = x;
    this->gen_01_dcblock_30_ym1 = y;
    return y;
}

void gen_01_dcblock_30_reset() {
    this->gen_01_dcblock_30_xm1 = 0;
    this->gen_01_dcblock_30_ym1 = 0;
}

void gen_01_dcblock_30_dspsetup() {
    this->gen_01_dcblock_30_reset();
}

number gen_01_dcblock_32_next(number x, number gain) {
    RNBO_UNUSED(gain);
    number y = x - this->gen_01_dcblock_32_xm1 + this->gen_01_dcblock_32_ym1 * 0.9997;
    this->gen_01_dcblock_32_xm1 = x;
    this->gen_01_dcblock_32_ym1 = y;
    return y;
}

void gen_01_dcblock_32_reset() {
    this->gen_01_dcblock_32_xm1 = 0;
    this->gen_01_dcblock_32_ym1 = 0;
}

void gen_01_dcblock_32_dspsetup() {
    this->gen_01_dcblock_32_reset();
}

void gen_01_dspsetup(bool force) {
    if ((bool)(this->gen_01_setupDone) && (bool)(!(bool)(force)))
        return;

    this->gen_01_setupDone = true;
    this->gen_01_delR_dspsetup();
    this->gen_01_delL_dspsetup();
    this->gen_01_RingBuff_exprdata_dspsetup();
    this->gen_01_latch_5_dspsetup();
    this->gen_01_latch_7_dspsetup();
    this->gen_01_phasor_16_dspsetup();
    this->gen_01_phasor_18_dspsetup();
    this->gen_01_dcblock_30_dspsetup();
    this->gen_01_dcblock_32_dspsetup();
}

void param_01_getPresetValue(PatcherStateInterface& preset) {
    preset["value"] = this->param_01_value;
}

void param_01_setPresetValue(PatcherStateInterface& preset) {
    if ((bool)(stateIsEmpty(preset)))
        return;

    this->param_01_value_set(preset["value"]);
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

Index globaltransport_getSampleOffset(MillisecondTime time) {
    return this->mstosamps(this->maximum(0, time - this->getEngine()->getCurrentTime()));
}

number globaltransport_getTempoAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_tempo[(Index)sampleOffset] : this->globaltransport_lastTempo);
}

number globaltransport_getStateAtSample(SampleIndex sampleOffset) {
    return (sampleOffset >= 0 && sampleOffset < this->vs ? this->globaltransport_state[(Index)sampleOffset] : this->globaltransport_lastState);
}

number globaltransport_getState(MillisecondTime time) {
    return this->globaltransport_getStateAtSample(this->globaltransport_getSampleOffset(time));
}

number globaltransport_getBeatTime(MillisecondTime time) {
    number i = 2;

    while (i < this->globaltransport_beatTimeChanges->length && this->globaltransport_beatTimeChanges[(Index)(i + 1)] <= time) {
        i += 2;
    }

    i -= 2;
    number beatTimeBase = this->globaltransport_beatTimeChanges[(Index)i];

    if (this->globaltransport_getState(time) == 0)
        return beatTimeBase;

    number beatTimeBaseMsTime = this->globaltransport_beatTimeChanges[(Index)(i + 1)];
    number diff = time - beatTimeBaseMsTime;
    return beatTimeBase + this->mstobeats(diff);
}

bool globaltransport_setTempo(MillisecondTime time, number tempo, bool notify) {
    if ((bool)(notify)) {
        this->processTempoEvent(time, tempo);
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getTempoAtSample(offset) != tempo) {
            this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
            this->globaltransport_beatTimeChanges->push(time);
            fillSignal(this->globaltransport_tempo, this->vs, tempo, offset);
            this->globaltransport_lastTempo = tempo;
            this->globaltransport_tempoNeedsReset = true;
            return true;
        }
    }

    return false;
}

number globaltransport_getTempo(MillisecondTime time) {
    return this->globaltransport_getTempoAtSample(this->globaltransport_getSampleOffset(time));
}

bool globaltransport_setState(MillisecondTime time, number state, bool notify) {
    if ((bool)(notify)) {
        this->processTransportEvent(time, TransportState(state));
        this->globaltransport_notify = true;
    } else {
        Index offset = (Index)(this->globaltransport_getSampleOffset(time));

        if (this->globaltransport_getStateAtSample(offset) != state) {
            fillSignal(this->globaltransport_state, this->vs, state, offset);
            this->globaltransport_lastState = TransportState(state);
            this->globaltransport_stateNeedsReset = true;

            if (state == 0) {
                this->globaltransport_beatTimeChanges->push(this->globaltransport_getBeatTime(time));
                this->globaltransport_beatTimeChanges->push(time);
            }

            return true;
        }
    }

    return false;
}

bool globaltransport_setBeatTime(MillisecondTime time, number beattime, bool notify) {
    if ((bool)(notify)) {
        this->processBeatTimeEvent(time, beattime);
        this->globaltransport_notify = true;
        return false;
    } else {
        bool beatTimeHasChanged = false;
        float oldBeatTime = (float)(this->globaltransport_getBeatTime(time));
        float newBeatTime = (float)(beattime);

        if (oldBeatTime != newBeatTime) {
            beatTimeHasChanged = true;
        }

        this->globaltransport_beatTimeChanges->push(beattime);
        this->globaltransport_beatTimeChanges->push(time);
        return beatTimeHasChanged;
    }
}

number globaltransport_getBeatTimeAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getBeatTime(this->getEngine()->getCurrentTime() + msOffset);
}

array<number, 2> globaltransport_getTimeSignature(MillisecondTime time) {
    number i = 3;

    while (i < this->globaltransport_timeSignatureChanges->length && this->globaltransport_timeSignatureChanges[(Index)(i + 2)] <= time) {
        i += 3;
    }

    i -= 3;

    return {
        this->globaltransport_timeSignatureChanges[(Index)i],
        this->globaltransport_timeSignatureChanges[(Index)(i + 1)]
    };
}

array<number, 2> globaltransport_getTimeSignatureAtSample(SampleIndex sampleOffset) {
    auto msOffset = this->sampstoms(sampleOffset);
    return this->globaltransport_getTimeSignature(this->getEngine()->getCurrentTime() + msOffset);
}

bool globaltransport_setTimeSignature(MillisecondTime time, number numerator, number denominator, bool notify) {
    if ((bool)(notify)) {
        this->processTimeSignatureEvent(time, (int)(numerator), (int)(denominator));
        this->globaltransport_notify = true;
    } else {
        array<number, 2> currentSig = this->globaltransport_getTimeSignature(time);

        if (currentSig[0] != numerator || currentSig[1] != denominator) {
            this->globaltransport_timeSignatureChanges->push(numerator);
            this->globaltransport_timeSignatureChanges->push(denominator);
            this->globaltransport_timeSignatureChanges->push(time);
            return true;
        }
    }

    return false;
}

void globaltransport_advance() {
    if ((bool)(this->globaltransport_tempoNeedsReset)) {
        fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
        this->globaltransport_tempoNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTempoEvent(this->globaltransport_lastTempo);
        }
    }

    if ((bool)(this->globaltransport_stateNeedsReset)) {
        fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
        this->globaltransport_stateNeedsReset = false;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTransportEvent(TransportState(this->globaltransport_lastState));
        }
    }

    if (this->globaltransport_beatTimeChanges->length > 2) {
        this->globaltransport_beatTimeChanges[0] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 2)];
        this->globaltransport_beatTimeChanges[1] = this->globaltransport_beatTimeChanges[(Index)(this->globaltransport_beatTimeChanges->length - 1)];
        this->globaltransport_beatTimeChanges->length = 2;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendBeatTimeEvent(this->globaltransport_beatTimeChanges[0]);
        }
    }

    if (this->globaltransport_timeSignatureChanges->length > 3) {
        this->globaltransport_timeSignatureChanges[0] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 3)];
        this->globaltransport_timeSignatureChanges[1] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 2)];
        this->globaltransport_timeSignatureChanges[2] = this->globaltransport_timeSignatureChanges[(Index)(this->globaltransport_timeSignatureChanges->length - 1)];
        this->globaltransport_timeSignatureChanges->length = 3;

        if ((bool)(this->globaltransport_notify)) {
            this->getEngine()->sendTimeSignatureEvent(
                (int)(this->globaltransport_timeSignatureChanges[0]),
                (int)(this->globaltransport_timeSignatureChanges[1])
            );
        }
    }

    this->globaltransport_notify = false;
}

void globaltransport_dspsetup(bool force) {
    if ((bool)(this->globaltransport_setupDone) && (bool)(!(bool)(force)))
        return;

    fillSignal(this->globaltransport_tempo, this->vs, this->globaltransport_lastTempo);
    this->globaltransport_tempoNeedsReset = false;
    fillSignal(this->globaltransport_state, this->vs, this->globaltransport_lastState);
    this->globaltransport_stateNeedsReset = false;
    this->globaltransport_setupDone = true;
}

bool stackprotect_check() {
    this->stackprotect_count++;

    if (this->stackprotect_count > 128) {
        console->log("STACK OVERFLOW DETECTED - stopped processing branch !");
        return true;
    }

    return false;
}

void updateTime(MillisecondTime time) {
    this->_currentTime = time;
    this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(rnbo_fround(this->msToSamps(time - this->getEngine()->getCurrentTime(), this->sr)));

    if (this->sampleOffsetIntoNextAudioBuffer >= (SampleIndex)(this->vs))
        this->sampleOffsetIntoNextAudioBuffer = (SampleIndex)(this->vs) - 1;

    if (this->sampleOffsetIntoNextAudioBuffer < 0)
        this->sampleOffsetIntoNextAudioBuffer = 0;
}

void assign_defaults()
{
    gen_01_in1 = 0;
    gen_01_in2 = 0;
    gen_01_FeedCoe = 0;
    gen_01_delTime = 0;
    gen_01_Mix = 0;
    gen_01_reverse = 0;
    gen_01_grainSpeed = 0;
    gen_01_movDur = 0;
    param_01_value = 50;
    param_02_value = 500;
    param_03_value = 1;
    param_04_value = 0.5;
    param_05_value = 100;
    param_06_value = 0;
    _currentTime = 0;
    audioProcessSampleCount = 0;
    sampleOffsetIntoNextAudioBuffer = 0;
    zeroBuffer = nullptr;
    dummyBuffer = nullptr;
    didAllocateSignals = 0;
    vs = 0;
    maxvs = 0;
    sr = 44100;
    invsr = 0.00002267573696;
    gen_01_delR__maxdelay = 0;
    gen_01_delR_sizemode = 0;
    gen_01_delR_wrap = 0;
    gen_01_delR_reader = 0;
    gen_01_delR_writer = 0;
    gen_01_delL__maxdelay = 0;
    gen_01_delL_sizemode = 0;
    gen_01_delL_wrap = 0;
    gen_01_delL_reader = 0;
    gen_01_delL_writer = 0;
    gen_01_PhaseDiff2_value = 0;
    gen_01_PhaseDiff1_value = 0;
    gen_01_lastPhaseReader2_value = 0;
    gen_01_lastPhaseReader1_value = 0;
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
    gen_01_dcblock_30_xm1 = 0;
    gen_01_dcblock_30_ym1 = 0;
    gen_01_dcblock_32_xm1 = 0;
    gen_01_dcblock_32_ym1 = 0;
    gen_01_setupDone = false;
    param_01_lastValue = 0;
    param_02_lastValue = 0;
    param_03_lastValue = 0;
    param_04_lastValue = 0;
    param_05_lastValue = 0;
    param_06_lastValue = 0;
    globaltransport_tempo = nullptr;
    globaltransport_tempoNeedsReset = false;
    globaltransport_lastTempo = 120;
    globaltransport_state = nullptr;
    globaltransport_stateNeedsReset = false;
    globaltransport_lastState = 0;
    globaltransport_beatTimeChanges = { 0, 0 };
    globaltransport_timeSignatureChanges = { 4, 4, 0 };
    globaltransport_notify = false;
    globaltransport_setupDone = false;
    stackprotect_count = 0;
    _voiceIndex = 0;
    _noteNumber = 0;
    isMuted = 1;
}

// member variables

    number gen_01_in1;
    number gen_01_in2;
    number gen_01_FeedCoe;
    number gen_01_delTime;
    number gen_01_Mix;
    number gen_01_reverse;
    number gen_01_grainSpeed;
    number gen_01_movDur;
    number param_01_value;
    number param_02_value;
    number param_03_value;
    number param_04_value;
    number param_05_value;
    number param_06_value;
    MillisecondTime _currentTime;
    SampleIndex audioProcessSampleCount;
    SampleIndex sampleOffsetIntoNextAudioBuffer;
    signal zeroBuffer;
    signal dummyBuffer;
    bool didAllocateSignals;
    Index vs;
    Index maxvs;
    number sr;
    number invsr;
    Float64BufferRef gen_01_delR_buffer;
    Index gen_01_delR__maxdelay;
    Int gen_01_delR_sizemode;
    Index gen_01_delR_wrap;
    Int gen_01_delR_reader;
    Int gen_01_delR_writer;
    Float64BufferRef gen_01_delL_buffer;
    Index gen_01_delL__maxdelay;
    Int gen_01_delL_sizemode;
    Index gen_01_delL_wrap;
    Int gen_01_delL_reader;
    Int gen_01_delL_writer;
    number gen_01_PhaseDiff2_value;
    number gen_01_PhaseDiff1_value;
    number gen_01_lastPhaseReader2_value;
    number gen_01_lastPhaseReader1_value;
    Float64BufferRef gen_01_RingBuff_exprdata_buffer;
    Float64BufferRef gen_01_RingBuff;
    int gen_01_counter_1_carry;
    number gen_01_counter_1_count;
    int gen_01_counter_3_carry;
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
    number gen_01_dcblock_30_xm1;
    number gen_01_dcblock_30_ym1;
    number gen_01_dcblock_32_xm1;
    number gen_01_dcblock_32_ym1;
    bool gen_01_setupDone;
    number param_01_lastValue;
    number param_02_lastValue;
    number param_03_lastValue;
    number param_04_lastValue;
    number param_05_lastValue;
    number param_06_lastValue;
    signal globaltransport_tempo;
    bool globaltransport_tempoNeedsReset;
    number globaltransport_lastTempo;
    signal globaltransport_state;
    bool globaltransport_stateNeedsReset;
    number globaltransport_lastState;
    list globaltransport_beatTimeChanges;
    list globaltransport_timeSignatureChanges;
    bool globaltransport_notify;
    bool globaltransport_setupDone;
    number stackprotect_count;
    DataRef RingBuff;
    DataRef gen_01_delR_bufferobj;
    DataRef gen_01_delL_bufferobj;
    Index _voiceIndex;
    Int _noteNumber;
    Index isMuted;
    indexlist paramInitIndices;
    indexlist paramInitOrder;

};

PatcherInterface* creaternbomatic()
{
    return new rnbomatic();
}

#ifndef RNBO_NO_PATCHERFACTORY

extern "C" PatcherFactoryFunctionPtr GetPatcherFactoryFunction(PlatformInterface* platformInterface)
#else

extern "C" PatcherFactoryFunctionPtr rnbomaticFactoryFunction(PlatformInterface* platformInterface)
#endif

{
    Platform::set(platformInterface);
    return creaternbomatic;
}

} // end RNBO namespace

