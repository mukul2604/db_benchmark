#include<iostream>
#include<sqlite3.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include<sys/time.h>
using namespace std;

int NINSERT = 20000;

void gen_random(char *s, const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    s[len] = 0;
}

void cleanup(char *database) {
  if(remove(database) != 0 )
      perror("Error deleting file");
  else
      puts("File successfully deleted");
}

int callback(void *p, int argc, char **argv, char **ax) {
  cout << "Callback is called\n";
  return 0;
}

int main (int argc, char* argv[]) {
  sqlite3 *db;
  char *errMsg = 0;
  int rc = 0;
  long double totalTime = 0;
  struct timeval start,end;
  
  if (argc != 2) {
    cout << "Usage : ./sqlite_test <Number of records>" << endl;
    exit(rc);
  } 
 
  NINSERT = atoi(argv[1]);

  cout << "NINSERT: " << NINSERT << endl;

  char database[] = "filehandle.db";
  rc = sqlite3_open(database, &db);

  if (rc) {
    cerr << "error";
  } else {
    cout << "Opened \"" << database << "\" successfully." << endl;
  }
  
  char sqlStmt[]  = "CREATE TABLE FILEHANDLE("	\
		    "ID1 CHAR(16) NOT NULL,"	\
		    "ID2 CHAR(16) NOT NULL);";

  rc = sqlite3_exec(db, sqlStmt, callback, 0, &errMsg);

  if (rc != SQLITE_OK) {
    cerr << "SQL ERROR" << endl;
    sqlite3_free(errMsg);
    cleanup(database);
    exit(rc);
  } else {
    cout << "Table created successfully" << endl;
  }
  
   
  for (int i = 0 ; i < NINSERT ; i++) {
    char *id1 = new char[17];
    char *id2 = new char[17];
  
    gen_random(id1, 16);
    gen_random(id2, 16);

    char sqlRecordStmt[100] = "INSERT INTO FILEHANDLE(ID1,ID2) VALUES ('";
    char midStmt[20] =  "','";
    char endStmt[20] = "');";
    
    strcat(sqlRecordStmt, id1);
    strcat(sqlRecordStmt, midStmt);
    strcat(sqlRecordStmt, id2);
    strcat(sqlRecordStmt, endStmt);
     
    gettimeofday(&start,NULL);

    rc = sqlite3_exec(db, sqlRecordStmt, callback, 0, &errMsg);

    gettimeofday(&end,NULL); 

    double myTime = (end.tv_sec+(double)end.tv_usec/1000000) -
       (start.tv_sec+(double)start.tv_usec/1000000);
    
    //cout << "Delta: " << myTime << endl; 
    totalTime += myTime;;
    
    if (rc != SQLITE_OK) {
      cerr << "SQL ERROR"  << endl;
      sqlite3_free(errMsg);
      cleanup(database);
      exit(rc);
    } else {
    //  cout << "Record inserted successfully" << endl;
    }
    delete[] id1;
    delete[] id2;
  }
  
  cout << "Total time for inserting " << NINSERT << " records: " 
       << totalTime << " seconds" << endl;
 
  cout << "Average time insert per record: " 
       << totalTime / NINSERT 
       << " seconds" << endl;

  sqlite3_close(db);
  cleanup(database);
  return 0;
}