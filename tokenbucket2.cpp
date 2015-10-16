#include <iostream>
#include <ctime>
#include <unistd.h>

using namespace std;

class TokenBucket {
public:
  TokenBucket(int cap, int tps) {
     capacity = cap;
     tokenps = tps;
     tokens = 0;
     time(&lasttime);
  }
  bool getToken() {
     time_t now;
     time(&now);
     double diff = difftime(now, lasttime);
     tokens += (int)diff * tokenps;
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
  time_t lasttime;
};

int main() {
  TokenBucket bucket(4, 4);
  int counter = 0;
  while(1) {
     sleep(1);
     cout << "#" << counter << ": ";
     counter++;
     for (int i = 0; i < 5; i++) {
        if (bucket.getToken()) cout << "1 ";
        else cout << "0 ";
     }
     cout << endl;
  }
  return 0;
}
