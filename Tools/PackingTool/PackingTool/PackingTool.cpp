// PackingTool.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "sqlite3.h"
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

int insert_file(const char *db_name, fs::path filepath, fs::path base)
{
	ifstream file(filepath, ios::in | ios::binary);
	if (!file) {
		cerr << "An error occurred opening the file\n";
		return 1;
	}
	file.seekg(0, ifstream::end);
	long long int size = file.tellg();
	file.seekg(0);

	char* buffer = new char[size];
	file.read(buffer, size);

	sqlite3 *db = NULL;
	int rc = sqlite3_open_v2(db_name, &db, SQLITE_OPEN_READWRITE, NULL);
	if (rc != SQLITE_OK) {
		cerr << "db open failed: " << sqlite3_errmsg(db) << endl;
	}
	else {
		sqlite3_stmt *stmt = NULL;
		char query[300];
		sprintf_s(query, "INSERT INTO files VALUES(NULL, '%ls', %lld, ?)", fs::relative(filepath, base).c_str(), size);
		
		rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
		if (rc != SQLITE_OK) {
			cerr << "prepare failed: " << sqlite3_errmsg(db) << endl;
		}
		else {
			// SQLITE_STATIC because the statement is finalized
			// before the buffer is freed:
			rc = sqlite3_bind_blob(stmt, 1, buffer, size, SQLITE_STATIC);
			if (rc != SQLITE_OK) {
				cerr << "bind failed: " << sqlite3_errmsg(db) << endl;
			}
			else {
				rc = sqlite3_step(stmt);
				if (rc != SQLITE_DONE)
					cerr << "execution failed: " << sqlite3_errmsg(db) << endl;
			}
		}
		sqlite3_finalize(stmt);
	}
	sqlite3_close(db);

	delete[] buffer;
}

void iterate_folder(const char *db, fs::path base) {
	for (const auto & entry : fs::recursive_directory_iterator(base)) {
		if (!fs::is_directory(entry)) 
			insert_file(db, entry, base);
	}
}

void create_database(const char *db_path) {
	sqlite3 *db = NULL;
	sqlite3_open_v2(db_path, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
	sqlite3_stmt *stmt = NULL;

	sqlite3_prepare_v2(db, "CREATE TABLE files ( id INTEGER PRIMARY KEY, name TEXT NOT NULL, size INTEGER, data BLOB); ", -1, &stmt, NULL);
	sqlite3_step(stmt);
	
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	}

int main(int argc, char *argv[]) {
	if (argc <= 2) {
		puts("Please provide path_to_folder and output_file");
		return 1; }

	create_database(argv[2]);
	iterate_folder(argv[2], fs::path(argv[1]));

	return 0; }