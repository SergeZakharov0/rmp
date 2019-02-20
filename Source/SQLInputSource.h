/*
  ==============================================================================

    SQLInputSource.h
    Created: 21 Feb 2019 12:01:55am
    Author:  serge

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SQLite/sqlite3.h"

class SQLInputSource : public InputSource {  
public:
    SQLInputSource(String file, String db);
    
    ~SQLInputSource();

    InputStream* createInputStream() override;
    InputStream* createInputStreamFor (const String& relatedItemPath) override;
    int64 hashCode() const override;

private:
    String dbname;
    String fileToRetrieve;
	String anotherFile;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SQLInputSource)
};
