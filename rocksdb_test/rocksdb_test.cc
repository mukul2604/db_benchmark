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
  long double totalTime = 0;
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

    gettimeofday(&start,NULL);
    //cout << key << "|" << value << endl;
    s = db->Put(WriteOptions(), key, value);
    gettimeofday(&end,NULL); 
    assert(s.ok());
    double recTime = (end.tv_sec+(double)end.tv_usec/1000000) -
       (start.tv_sec+(double)start.tv_usec/1000000);
    totalTime +=  recTime;
    delete[] id1;
    delete[] id2;
  }

  cout << "Total time for inserting " << NINSERT << " records: " 
       << totalTime << " seconds" << endl;
 
  cout << "Average time insert per record: " 
       << totalTime / NINSERT 
       << " seconds" << endl;
  delete db;

  return 0;
}
