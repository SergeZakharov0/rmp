
#include "PitchShifter.h"
#include <math.h>


PitchShifter::PitchShifter()
{
	
}

PitchShifter::~PitchShifter()
{

}

void PitchShifter::stretch(const float *in_buff_ptr, float *result, float r_factor)
{
	//hanning_window
    dsp::WindowingFunction<float> hann_window { (size_t)window_size, dsp::WindowingFunction<float>::hann };
    dsp::FFT forwardFFT(fft_order);
    
    memset((void *) phase, 0, sizeof(float)*window_size );
    memset( (void *) result, 0, sizeof(float));
    
    for(int idx = 0; idx <= numSamples - (window_size + overlap); idx += overlap*r_factor )
    {
        //prepare buffer
        float windowed_buffer[2][window_size] = {0};
        memcpy( (void *)windowed_buffer[0], (void *)(in_buff_ptr + idx), sizeof(float) );
        memcpy( (void *)windowed_buffer[1], (void *)(in_buff_ptr + idx + overlap), sizeof(float) );
        hann_window.multiplyWithWindowingTable(windowed_buffer[0], window_size);
        hann_window.multiplyWithWindowingTable(windowed_buffer[1], window_size);

        dsp::Complex<float> cmpl_w_buffer[2][window_size];
        for(int i = 0; i < window_size; ++i)
        {
            cmpl_w_buffer[0][i] = windowed_buffer[0][i];
            cmpl_w_buffer[1][i] = windowed_buffer[1][i];
        }
        
        
        //fft performing
        dsp::Complex<float> fft[2][window_size] = {0};
        forwardFFT.perform(cmpl_w_buffer[0], fft[0], false);
        forwardFFT.perform(cmpl_w_buffer[1], fft[1], false);
        
        
        //phase computation
        dsp::Complex<float> fft_rephased[window_size];
        dsp::Complex<float> r_fft;
        float fMag;
        float fPhase;
        float fsMag;
        for(int i = 0; i < window_size; ++i)
        {
            float sign = phase[i] > 0 ? 1.0f : -1.0f;
            r_fft = fft[1][i]/fft[0][i];
            fPhase = atan2( r_fft.imag(), r_fft.real() );
            float tmp = phase[i] + fPhase;
            phase[i] = std::fmod(tmp, pi_twice); 
            
            fsMag = std::powf(fft[1][i].real(),2);
            fsMag += std::powf(fft[1][i].imag(), 2);
            fMag = sqrtf(fsMag);
            
            dsp::Complex<float> cmpl_tmp( fMag*cosf( phase[i]), fMag*sinf(phase[i])  );
            fft_rephased[i] = cmpl_tmp;
        }
        
        dsp::Complex<float> cmpl_result[window_size];
        float tmp_result[window_size];
        forwardFFT.perform(fft_rephased, cmpl_result, true);
        
        //complex_float to float
        for(int i = 0; i < window_size; ++i)
        {
            tmp_result[i] = cmpl_result[i].real();
            
        }
        hann_window.multiplyWithWindowingTable(tmp_result, window_size);
        
        
        //add to result buffer
        for(int i = 0; i < window_size; ++i)
        {
            int idx2 = idx/overlap;
            result[idx2 + i] = tmp_result[i];
        }
        
    }
    
    //norm
    float maxVal = result[0];
    for(int idx = 0; idx < resultSize; ++idx)
    {
        if(result[idx] > maxVal)
        {
            maxVal = result[idx];
        }
    }
    
    for(int idx = 0; idx < resultSize; ++idx)
    {
        result[idx] = 0.6f * result[idx]/maxVal;
    }
}

void PitchShifter::changeSpeed(const float *result, float * out_buff_ptr, float factor)
{
    int i = 0;
    for(double fidx = 0.0f; fidx < numSamples; fidx += factor, ++i)
    {
        int idx = (int)(fidx);
        out_buff_ptr[i] = result[idx];
    }
}

void PitchShifter::shiftTone(const float * in_buff_ptr, float * out_buff_ptr, float semitone_val)
{
    float factor = pow( 2, semitone_val/12.0f ); 
    float r_factor = 1.0/factor;
    resultSize = numSamples*factor + window_size;
    float *result = new float[resultSize];
    
    //stretch(in_buff_ptr, result, r_factor);
    changeSpeed(in_buff_ptr, out_buff_ptr, factor);
    
    delete [] result;
}

void PitchShifter::lazyShiftTone(const float * in_buff_ptr, float * out_buff_ptr, float semitone_val)
{
    resultSize = _resultSize;
	FILE *debug = fopen("info.txt", "a+");
	fprintf(debug,"%f: %d\n", ((float)_resultSize) / (1024 * 1024), (int) semitone_val);
	fclose(debug);
    changeSpeed(in_buff_ptr, out_buff_ptr, _factor);
}


std::shared_ptr<AudioBuffer<float>> PitchShifter::transposeBuffer(std::shared_ptr<AudioBuffer<float> > Input, int diffNote)
{

  
    AudioBuffer<float> *input_buffer =  Input.get() ;
    numSamples = input_buffer->getNumSamples();
    _factor = pow( 2, (float)diffNote/12.0f );
    _resultSize = (int)( numSamples / _factor);
    
    std::shared_ptr<AudioBuffer<float>> Output;
    Output.reset( new AudioBuffer<float>(2, _resultSize) );
    
    AudioBuffer<float> *out_buffer =  Output.get();
    const float * in_buff_ptr[2] = {input_buffer->getReadPointer(0), input_buffer->getReadPointer(1) };   
          float *out_buff_ptr[2] = { out_buffer->getWritePointer(0), out_buffer->getWritePointer(1) };

    lazyShiftTone(in_buff_ptr[0], out_buff_ptr[0], diffNote);
    lazyShiftTone(in_buff_ptr[1], out_buff_ptr[1], diffNote);
    return Output;
}
