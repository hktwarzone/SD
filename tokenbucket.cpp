#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
using namespace std::this_thread;

class TokenBucket {
public:
  TokenBucket(int cap, int tps) {
     capacity = cap;
     tokenps = tps;
     tokens = 0;
     lasttime = high_resolution_clock::now();
  }
  bool getToken() {
     auto now = high_resolution_clock::now();
     tokens += (int)(duration_cast<milliseconds>(now-lasttime).count()* tokenps / 1000);
     if (tokens > capacity) tokens = capacity;
     lasttime = now;
     if (tokens < 1) return false;
     tokens--;
     return true;
  }
private:
  int capacity;
  int tokenps;
  int tokens;
  high_resolution_clock::time_point lasttime;
};

int main() {
  TokenBucket bucket(4, 4);
  sleep_for(milliseconds(1000));
  for (int i = 0; i < 5; i++) {
    if (bucket.getToken()) cout << "1" << endl;
    else cout << "0" << endl;
  }
  sleep_for(milliseconds(1000));
  for (int i = 0; i < 5; i++) {
    if (bucket.getToken()) cout << "1" << endl;
    else cout << "0" << endl;
  }
  return 0;
}
