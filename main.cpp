#include <iostream>
#include <thread>
# include <mutex>
# include <vector>
#include "condition_variable"
# include <functional>
# include <list>
using namespace std;

class Workers
{
private:
    int amountOfThreads;
    vector<thread> threads;
    list<function<void()>> tasks;
    mutex tasks_mutex;
    condition_variable cv;
    bool notified = true;

public:
    Workers(int x)
    {
        setAmountOfThreads(x);
    }
    void setAmountOfThreads(int amount)
    {
        amountOfThreads = amount;
    }
    //creates given amount of threads and assign tasks to them
    void start(){
        for (int i = 0; i < amountOfThreads; ++i) {
            threads.emplace_back([&] {
                while (notified) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(tasks_mutex);
                        cv.wait(lock);
                        if (!tasks.empty()) {
                            task = *tasks.begin(); // Copy task for later use
                            tasks.pop_front(); // Remove task from list
                        }
                    }
                    if (task) {
                        task();
                    }
                    cv.notify_one();
                }
            });
        }

    }
    //add tasks into tasks list
    void post(const function<void()> & f){
        unique_lock<mutex> lock(tasks_mutex);
        tasks.emplace_back(f);
        lock.unlock();
        cv.notify_one();
    }
    // terminates all threads after all the tasks is done
    void join(){
        unique_lock<mutex> lock(tasks_mutex);
        if(!tasks.empty()){
            //checks if the lock has been released and a lambda expression for if all the tasks has been done.
            //"this" indicated on that we are using the defined variables above:)
            cv.wait(lock, [this]{
                if(tasks.empty()){
                    return true;
                }
                else{
                    cv.notify_one();
                    return false;
                }
            });
        }
        notified = false;
        lock.unlock();
        cv.notify_all();
        for (int i = 0; i < threads.size(); ++i) {
            if(threads[i].joinable()){
                threads[i].join();
            }
        }
    }
    // pauses from a task to me done by given time (seconds)
    void post_timout(const function<void()> & f, int seconds){
        function<void()> task = [seconds, f]{
            this_thread::sleep_for(chrono::seconds(seconds));
            f();
        };
        post(task);
    }
    //This stop function stops everything, which means that it terminates all threads independent
    //of if it is any tasks left
    void stop(){
        unique_lock<mutex> lock(tasks_mutex);
        notified = false;
        lock.unlock();
        cv.notify_all();
        for (int i = 0; i < threads.size(); ++i) {
            if(threads[i].joinable()){
                threads[i].join();
            }
        }
    }
};

int main() {
    int seconds2 = 2;
    int seconds3 = 3;

    Workers worker_threads(4);
    Workers event_loop(1);

    function<void()> taskA = []{
        cout<<"Task A done by worker thread\n"<<endl;
    };
    function<void()> taskB = []{
        cout<<"Task B by worker thread\n"<<endl;
    };
    function<void()> taskC = []{
        cout<<"Task C by event loop\n"<<endl;
    };
    function<void()> taskD = []{
        cout<<"Task D by event loop\n"<<endl;
    };
    function<void()> task2 = [&seconds2]{
        cout << "Task done by worker thread after " << seconds2 << " seconds\n" << endl;
    };
    function<void()> task3 = [&seconds3]{
        cout << "Task done by worker thread after " << seconds3 << " seconds\n" << endl;
    };

    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread

    worker_threads.post(taskA);
    worker_threads.post(taskB);
    event_loop.post(taskC);
    event_loop.post(taskD);

    worker_threads.post_timout(task3,seconds3);
    worker_threads.post_timout(task2,seconds2);

    worker_threads.join(); // Calls join() on the worker threads
    event_loop.join(); // Calls join() on the event thread
}