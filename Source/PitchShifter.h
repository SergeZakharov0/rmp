#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class PitchShifter
{
	public:
	    PitchShifter(int length);
	    ~PitchShifter();
	
		//=================================================
	    void applyShifter(std::shared_ptr<AudioBuffer<float> > Input, std::shared_ptr<AudioBuffer<float> > Output[128][128], 
                                                                        int lowNote, int highNote, int lowVel, int highVel);
	
        void applyLazyShifter(std::shared_ptr<AudioBuffer<float> > Input, std::shared_ptr<AudioBuffer<float> > Output[128][128], 
                                                                        int lowNote, int highNote, int lowVel, int highVel);
        
	private:
	//=====================================================
	    void stretch(const float *in_buff_ptr, float *result, float r_factor);
	    void changeSpeed(const float *result, float * out_buff_ptr, float factor);
	    void shiftTone(const float * in_buff_ptr, float * out_buff_ptr, float semitone_val);
	    
    //=====================================================

        static const size_t window_size = 4096;
 		const size_t overlap = window_size/4;
 		const int fft_order = 12;
 		const float pi_twice = 2.0f*3.14159265358979323846f;

   		float phase[window_size] = {0};
   		int numSamples;
   		int resultSize;

};



