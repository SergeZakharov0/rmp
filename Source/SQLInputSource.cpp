/*
  ==============================================================================

    SQLInputSource.cpp
    Created: 21 Feb 2019 12:01:55am
    Author:  serge

  ==============================================================================
*/

#include "SQLInputSource.h"
#include <stdlib.h>

SQLInputSource::SQLInputSource(String _file, String _db) {
    dbname = _db;
	fileToRetrieve = _file;
	anotherFile = "";
    }

SQLInputSource::~SQLInputSource() {
	}

InputStream* SQLInputSource::createInputStream() {
    sqlite3 *db;
    sqlite3_stmt *res;
	int rc;
	rc = sqlite3_open(dbname.toRawUTF8(), &db);
	char queue[256];
	strcpy(queue, "SELECT data, size FROM files WHERE name = '");
	if (anotherFile == "")
		strcat(queue, fileToRetrieve.toRawUTF8());
	else
		strcat(queue, anotherFile.toRawUTF8());
	strcat(queue, "'");
	rc = sqlite3_prepare_v2(db, queue, -1, &res, 0);
	rc = sqlite3_step(res);

	const void *data = sqlite3_column_blob(res, 0);
	int size = sqlite3_column_int(res, 1);
	MemoryInputStream *stream = new MemoryInputStream(data, size, true);

	rc = sqlite3_finalize(res);
	rc = sqlite3_close(db);

	return stream;
	}

InputStream* SQLInputSource::createInputStreamFor(const String& relatedItemPath) {
	anotherFile = relatedItemPath;
	InputStream *res = createInputStream();
	anotherFile = "";
	return res;
	}

int64 SQLInputSource::hashCode() const {
	return 0;
}
