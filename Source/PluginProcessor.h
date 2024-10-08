/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

enum Slope
{
  Slope_12,
  Slope_24,
  Slope_36,
  Slope_48,
};

// data stucture to extract parameters from apvts
struct ChainSettings
{
  float peakFreq {0}, peakGainInDecibels {0}, peakQuality {1.f};
  float lowCutFreq {0}, highCutFreq {0};
  Slope lowCutSlope {Slope::Slope_12}, highCutSlope {Slope::Slope_12};
};

// helper function
ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts);

//==============================================================================
/**
*/
class SimpleEQAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SimpleEQAudioProcessor();
    ~SimpleEQAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //this thingie made static coz no member variables
    static juce::AudioProcessorValueTreeState::ParameterLayout
      createParameterLayout();

    juce::AudioProcessorValueTreeState apvts{
      *this,nullptr,"Parameters",createParameterLayout()
    };

private:
    // namespace
    using Filter = juce::dsp::IIR::Filter<float>;
    
    //processing context
    using CutFilter = juce::dsp::ProcessorChain<Filter,Filter,Filter,Filter>; 

    using Monochain = juce::dsp::ProcessorChain<CutFilter,Filter,CutFilter>;

    Monochain leftChain, rightChain; //two channels for stereo

    enum ChainPositions
    {
      LowCut,
      Peak,
      HighCut
    };

    void updatePeakFilter(const ChainSettings& chainSettings);
    using Coefficients = Filter::CoefficientsPtr;
    static void updateCoefficients(Coefficients& old, const Coefficients& replacements);
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessor)
};
