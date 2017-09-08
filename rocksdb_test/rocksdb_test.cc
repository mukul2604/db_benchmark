#include <cstdio>
#include <string>
#include <iostream>
#include <rocksdb/db.h>
#include <rocksdb/slice.h>
#include <rocksdb/options.h>
#include<sys/time.h>

using namespace rocksdb;
using namespace std;
string kDBPath = "./filehandle_rocksdb";
int NINSERT = 0;

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

int main(int argc, char* argv[]) {
  DB* db;
  Options options;
  struct timeval start,end;
  long double totalInsertTime = 0, totalDeleteTime = 0;
  vector<string> keys;

  if (argc != 2) {
    cout << "Usage : ./rocksdb_test <Number of records>" << endl;
    exit(-1);
  } 
  NINSERT = atoi(argv[1]);
  cout << "NINSERT: " << NINSERT << endl;
  // TODO: remove the DB if already present

  // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
  options.IncreaseParallelism();
  options.OptimizeLevelStyleCompaction();
  // create the DB if it's not already present
  options.create_if_missing = true;

  // open DB
  DestroyDB(kDBPath, options);
  Status s = DB::Open(options, kDBPath, &db);
  assert(s.ok());

  // Put key-value
  for (int i = 0 ; i < NINSERT; i++) {
    char *id1 = new char[17];
    char *id2 = new char[17];
  
    gen_random(id1, 16);
    gen_random(id2, 16);

    string key(id1); 
    string value(id2);

    //cout << key << "|" << value << endl;
    WriteOptions woptions = WriteOptions();
    woptions.sync = true;
    gettimeofday(&start,NULL);
    s = db->Put(woptions, key, value);
    gettimeofday(&end,NULL); 
    assert(s.ok());
    double recTime = (end.tv_sec+(double)end.tv_usec/1000000) -
       (start.tv_sec+(double)start.tv_usec/1000000);
    totalInsertTime +=  recTime;
    delete[] id1;
    delete[] id2;
    keys.push_back(key);
  }

  for (string key : keys) {
    string value;
    s = db->Get(ReadOptions(), key, &value);
    assert(s.ok());

    WriteOptions woptions = WriteOptions();
    woptions.sync = true;

    gettimeofday(&start,NULL);
    s = db->Delete(woptions, key);
    gettimeofday(&end,NULL);
 
    assert(s.ok());

    double recTime = (end.tv_sec+(double)end.tv_usec/1000000) -
       (start.tv_sec+(double)start.tv_usec/1000000);
    totalDeleteTime += recTime;

    s = db->Get(ReadOptions(), key, &value);
    assert(!s.ok());
  }

  cout << "Total time for inserting " << NINSERT << " records: " 
       << totalInsertTime << " seconds" << endl;
 
  cout << "Average time insert per record: " 
       << totalInsertTime / NINSERT 
       << " seconds" << endl;

  cout << "Total time for deleting " << NINSERT << " records: " 
       << totalDeleteTime << " seconds" << endl;
 
  cout << "Average time delete per record: " 
       << totalDeleteTime / NINSERT 
       << " seconds" << endl;
  delete db;
  
  return 0;
}
