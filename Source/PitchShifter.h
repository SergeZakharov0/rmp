#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class PitchShifter
{
	public:
	    PitchShifter();
	    ~PitchShifter();
	
		//=================================================
	    std::shared_ptr<AudioBuffer<float>> transposeBuffer(std::shared_ptr<AudioBuffer<float> > Input, int diffNote);
	
        void lazyShiftTone(const float * in_buff_ptr, float * out_buff_ptr, float semitone_val);
        
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
   		int _resultSize;
   		float _factor;

};



