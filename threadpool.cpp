// implement a thread pool that can execute all types of functions
// reference: https://gist.github.com/jl2/889430

#include <iostream>
#include <queue>
#include <pthread.h>

using namespace std;

class Task {
public:
   Task(){}
   virtual ~Task() {}
   virtual void run() = 0;
};

static pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;

class FibTask : public Task {
public:
   FibTask(int n): Task(), _n(n) {}
   ~FibTask() {}
   virtual void run() {
      long long val = innerFib(_n);
      pthread_mutex_lock(&console_mutex);
      cout << "Fibd " << _n << " = " << val << endl;
      pthread_mutex_unlock(&console_mutex);
   }
private:
   long long _n;
   long long innerFib(long long n) {
      if (n <= 1) return 1;
      return innerFib(n - 1) + innerFib(n - 2);
   }
};


class TaskQ {
public:
   TaskQ() {
      finished = false;
      pthread_mutex_init(&qmtx, 0);
      pthread_cond_init(&wcond, 0);
   }
   ~TaskQ() {
      pthread_mutex_destroy(&qmtx);
      pthread_cond_destroy(&wcond);
   }
   void addTask(Task* nt) {
      if (!finished) {
         pthread_mutex_lock(&qmtx);
         tasks.push(nt);
         pthread_cond_signal(&wcond);
         pthread_mutex_unlock(&qmtx);
      }
   }
   Task* getTask() {
      Task* nt = 0;
      pthread_mutex_lock(&qmtx);
      if (finished && tasks.empty()) nt = 0;
      else {
         if (tasks.empty()) pthread_cond_wait(&wcond, &qmtx);
         nt = tasks.front();
         tasks.pop();
      }
      pthread_mutex_unlock(&qmtx);
      return nt;
   }
   void finish() {
      pthread_mutex_lock(&qmtx);
      finished = true;
      pthread_cond_signal(&wcond);
      pthread_mutex_unlock(&qmtx);
   }
   bool hasTask() { return tasks.size() > 0; }
private:
   bool finished;
   queue<Task*> tasks;
   pthread_mutex_t qmtx;
   pthread_cond_t wcond;
};

class ThreadPool {
public:
   ThreadPool(int n) : _numthreads(n) {
      threads = new pthread_t[n];
      for (int i = 0; i < _numthreads; i++) {
         pthread_create(&(threads[i]), 0, ThreadPool::getWork, &taskq);
      }
   }
   ~ThreadPool() {
      finish();
      waitForFinish();
      for (int i = 0; i < _numthreads; i++) {
         pthread_join(threads[i], 0);
      }
      delete [] threads;
   }
   static void* getWork(void* param) {
      Task* nt = 0;
      TaskQ* taskq = (TaskQ*)param;
      while(nt = taskq->getTask()) {
         nt->run();
         delete nt;
      }
      return 0;
   }
   void addTask(Task* nt) {
      taskq.addTask(nt);
   }
   void finish() {
      taskq.finish();
   }
   void waitForFinish() {
      while(taskq.hasTask()) {}
   }
private:
   int _numthreads;
   pthread_t* threads;
   TaskQ taskq;
};

int main(int argc, char* argv[]) {
   ThreadPool *tp = new ThreadPool(12);
   for (int i = 1; i <= 100; i++) {
      int rv = rand() % 50 + 1;
      tp->addTask(new FibTask(rv));
   }
   tp->finish();
   delete tp;
   return 0;
}
