/* 
ChucK program used with ChucK => Max in fmenv-ampmmod-osc.maxpat
Includes
    • global variables
    • public class FMenv 
        - main signal chain objects
        - functions to modify signal chain object values
    • Run in local mode (without Max) for testing
        - set localOn to 1
        - adjust initialize values as needed
    • Event with eventHandler function
*/

0 => int localOn; //for running program outside of Max

public class FMenv extends UGen
{
    SinOsc mod => PowerADSR modEnv => SinOsc car => ADSR carEnv;
    2 => car.sync; //for FM synth

    //FUNCTIONS
    fun void setModEnv(dur attack, dur decay, float susLev, dur rel)
        {  modEnv.set(attack, decay, susLev, rel); }

    fun void setModEnvCurves(float attackCur, float decayCur, float relCur)
        { modEnv.setCurves(attackCur, decayCur, relCur); }

    fun void setCarEnv(dur attack, dur decay, float susLev, dur rel)
        { carEnv.set(attack, decay, susLev, rel); }
    
    fun void setGain(float gainIn)  
        { gainIn => car.gain; }

    fun void setFreq(float freqIn) 
        { freqIn => car.freq; }

    fun void assignFM(float harm, float modIndex)
    {
        car.freq() * harm => mod.freq;
        mod.freq() * modIndex => mod.gain;
    }
}

//
NRev rev => Dyno dy => dac;

//dsp settings
dy.attackTime(1::ms); // sets time for limiter to kick in
dy.thresh(0.8); // sets limiter threshold

//global variables (set in Max/MSP)
global int synthOn;
global float carFreq;
global float carGain;
global float harm;
global float modIndex;
global float envDurMs;
global int   gapTimeMs;
global float revMix;

//FM modulator PowerADSR envelope and envelop curves
global float gModAttPrcnt;
global float gModDecPrcnt; 
global float gModRelPrcnt; 
global float gModSusLev;
global float gModAttCrv; //attack curve .01 to 8
global float gModDecCrv;
global float gModRelCrv;

//carrier env
global int carEnvDiv; //controls the attack/decay time as fraction of envelope duration

dur envDur; //event duration value

//initialize if running locally
if (localOn == 1)
{
    1 => synthOn;
    0.2 => carGain;
    110 => carFreq;
    1.69 => harm;
    8 => modIndex;
    3000 => envDurMs;
    0.5 => gModAttPrcnt => gModDecPrcnt; 
    0.25 => gModRelPrcnt;
    1 => gModAttCrv => gModDecCrv => gModRelCrv;
    1000 => gapTimeMs;
    8 => carEnvDiv;
    0.2 => revMix;
}
//else { 0 => car.gain; }

fun void eventHandler(Event eIn, FMenv FMin, int id)
{
   while(true)
    {
        //initilialize
        time envEnd; 10::ms => dur loopInc;
        if (carEnvDiv == 0) 8 => carEnvDiv;
        eIn => now;

        if (synthOn == 1)
        {
            //initialize
            FMin.setFreq(carFreq);
            FMin.setGain(carGain);
            FMin.assignFM(harm, modIndex);
            revMix => rev.mix;

            // set envelope values
            envDurMs::ms => envDur;

            //carrier envelope 
            //  attack and decay values use envelop duration divided by carEnvDiv
            //  release time is scaled by modulator envelope rel %
            FMin.setCarEnv(envDur/carEnvDiv, envDur - (envDur/carEnvDiv), 1, envDur * gModRelPrcnt);
            FMin.setModEnv(envDur * gModAttPrcnt, envDur * gModDecPrcnt, gModSusLev, envDur * gModRelPrcnt);
            FMin.setModEnvCurves(gModAttCrv, gModDecCrv, gModRelCrv);

            //connect the synth object signal chain to reverb and dac output;
            FMin.carEnv => rev;

            now + envDur => envEnd;
            //<<< "envelopes on", id >>>;
            FMin.carEnv.keyOn(); //<<< "carEnv on" >>>;
            FMin.modEnv.keyOn();

            while (now < envEnd) //envDur => now;
            {
                FMin.setFreq(carFreq);
                FMin.setGain(carGain);
                FMin.assignFM(harm, modIndex);
                loopInc => now;
            }
            
            FMin.modEnv.keyOff(); //<<< "keyOff" >>>;
            FMin.carEnv.keyOff(); //turn off carrier sound before gap time
            
            //release time and gap time added
            //FMin.modEnv.releaseTime() => now; //<<< "mod env Release done" >>>;
            now + FMin.modEnv.releaseTime() => envEnd;
            while (now < envEnd) 
            {
                FMin.setFreq(carFreq);
                FMin.assignFM(harm, modIndex);
                loopInc => now;
            }
            //sets a break before the next envelope - not useful w Tidal
            gapTimeMs::ms => now; 
            
            //disconnect from dac
            FMin.carEnv =< rev;
        }
        else { FMin.setGain(0); 100::ms => now; }
    }
}

//events
global Event e;
FMenv FM1, FM2, FM3, FM4, FM5;

spork ~ eventHandler(e, FM1, 1);
spork ~ eventHandler(e, FM2, 2);
spork ~ eventHandler(e, FM3, 3);
spork ~ eventHandler(e, FM4, 4);
spork ~ eventHandler(e, FM5, 5);

while (true)
{
    6::second => now;
    if (localOn == 1) e.signal(); //generate event signal when running locally
}

