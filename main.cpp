#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <unistd.h>

using namespace std;

#define SECOND_SLEEP 1000000

template<class T>
class Channel {
private:
    std::queue<T> buffer;
    size_t capacity;
    mutex buffer_mutex;
public:
    explicit Channel(size_t c) : capacity(c) {}

    void operator<<(const T obj) {
        while (buffer.size() >= capacity) {
            std::this_thread::yield();
        }
        std::unique_lock<mutex> lock(buffer_mutex);
        buffer.push(obj);
    }

    void operator>>(T &obj) {
        while (buffer.empty() || !buffer_mutex.try_lock() || buffer.empty())
            std::this_thread::yield();
        obj = buffer.front();
        buffer.pop();
        buffer_mutex.unlock();
    }

    const size_t getLength() const {
        return buffer.size();
    }

    const size_t &getCapacity() const {
        return capacity;
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
        cout << ch.getLength() << " : " << s << " : " << ch.getLength() << " : " << ch.getCapacity() << endl;
        usleep(2 * SECOND_SLEEP);
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