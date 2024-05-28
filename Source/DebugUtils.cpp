/*
  ==============================================================================

    DebugUtils.cpp
    Created: 2 May 2024 11:44:17am
    Author:  Jonatan

  ==============================================================================
*/

#include "DebugUtils.h"

#if JUCE_WINDOWS
#include <Windows.h>
void debugLog(const juce::String& message) {
    OutputDebugString(("[VirtuosoCompanion] " + message).toUTF8());
}
#else
void debugLog(const juce::String& message) {
    DBG(message);
}
#endif
