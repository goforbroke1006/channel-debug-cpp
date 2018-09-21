#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <queue>
#include <unistd.h>

using namespace std;

template<class T>
class Channel {
private:
    std::queue<T> buffer;
    size_t capacity;
    mutex buffer_mutex;
public:
    explicit Channel(size_t c) : capacity(c) {}

    void operator<<(const T obj) {
        while (buffer.size() == capacity)
            std::this_thread::yield();
        while (!buffer_mutex.try_lock())
            std::this_thread::yield();
        buffer.push(obj);
        buffer_mutex.unlock();
    }

    void operator>>(T &obj) {
        while (buffer.empty())
            std::this_thread::yield();
        unique_lock<mutex> lock(buffer_mutex);
        obj = buffer.front();
        buffer.pop();
    }
};

void writeRoutine(Channel<string> &ch, __useconds_t delay, int index) {
    while (true) {
        ch << ("hello " + std::to_string(index));
        usleep(delay);
    }
}

void readRoutine(Channel<string> &ch) {
    string s;
    while (true) {
        ch >> s;
        cout << s << endl;
    }
}

int main(int argc, char **argv) {
    Channel<string> ch(5);

    std::vector<thread> tasks;

    tasks.emplace_back(writeRoutine, std::ref(ch), 500, 1);
    tasks.emplace_back(writeRoutine, std::ref(ch), 300, 2);
    tasks.emplace_back(writeRoutine, std::ref(ch), 250, 3);

    tasks.emplace_back(readRoutine, std::ref(ch));

    for (thread &t : tasks)
        t.join();

    return 0;
}