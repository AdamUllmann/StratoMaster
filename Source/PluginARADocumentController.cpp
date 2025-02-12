#include "PluginARADocumentController.h"
#include "PluginARAPlaybackRenderer.h"

//==============================================================================
juce::ARAPlaybackRenderer* StratomasterDocumentController::doCreatePlaybackRenderer() noexcept
{
    return new StratomasterPlaybackRenderer (getDocumentController());
}

//==============================================================================
bool StratomasterDocumentController::doRestoreObjectsFromStream (juce::ARAInputStream& input, const juce::ARARestoreObjectsFilter* filter) noexcept
{
    return true;
}

bool StratomasterDocumentController::doStoreObjectsToStream (juce::ARAOutputStream& output, const juce::ARAStoreObjectsFilter* filter) noexcept
{
    return true;
}

//==============================================================================
const ARA::ARAFactory* JUCE_CALLTYPE createARAFactory()
{
    return juce::ARADocumentControllerSpecialisation::createARAFactory<StratomasterDocumentController>();
}
