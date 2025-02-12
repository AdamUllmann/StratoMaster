#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class StratomasterDocumentController  : public juce::ARADocumentControllerSpecialisation
{
public:
    //==============================================================================
    using ARADocumentControllerSpecialisation::ARADocumentControllerSpecialisation;

protected:
    //==============================================================================
    // Override document controller customization methods here

    juce::ARAPlaybackRenderer* doCreatePlaybackRenderer() noexcept override;

    bool doRestoreObjectsFromStream (juce::ARAInputStream& input, const juce::ARARestoreObjectsFilter* filter) noexcept override;
    bool doStoreObjectsToStream (juce::ARAOutputStream& output, const juce::ARAStoreObjectsFilter* filter) noexcept override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StratomasterDocumentController)
};
