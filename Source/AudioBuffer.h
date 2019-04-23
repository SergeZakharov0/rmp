
template <typename datatype>
class rmpAudioBuffer
{
public:
    rmpAudioBuffer();
    rmpAudioBuffer(int numberOfChannels, size_t numSamples, bool clearBuffers = false);
    ~rmpAudioBuffer();

protected:
    shared_ptr<datatype> 

};