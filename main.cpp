#include <iostream>
#include <condition_variable>
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
    std::condition_variable cve;
    std::condition_variable cvf;
public:
    explicit Channel(size_t c) : capacity(c) {}

    /**
     * write to channel
     */
    void operator<<(const T obj) {
        unique_lock<mutex> lock(buffer_mutex);
        cvf.wait(lock, [this] { return getLength() < getCapacity(); });
        buffer.push(obj);
        cve.notify_one();
    }

    /**
     * read from channel
     */
    void operator>>(T &obj) {
        unique_lock<mutex> lock(buffer_mutex);
        cve.wait(lock, [this] { return getLength() > 0; });
        obj = buffer.front();
        buffer.pop();
        cvf.notify_one();
    }

    const size_t getLength() const {
        return buffer.size();
    }

    const size_t &getCapacity() const {
        return capacity;
    }

    const bool isFull() const {
        return getLength() == getCapacity();
    }

    const bool isEmpty() const {
        return getLength() == 0;
    }
};

void writeRoutine(Channel<string> &ch, __useconds_t delay, int index, unsigned int fakeValuesCount) {
    for (int i = 0; i < fakeValuesCount; ++i) {
        ch << ("hello " + to_string(index));
        usleep(delay);
    }
}

void readRoutine(Channel<string> &ch) {
    string s;
    while (true) {
        cout << ch.getLength();
        cout << " (" << (ch.isFull() ? "full" : "not full") << ") ";
        ch >> s;
        cout << " : ";
        cout << s;
        cout << " : " << ch.getLength() << " : " << ch.getCapacity();
        cout << endl;
        usleep(0.5 * SECOND_SLEEP);
    }
}

int main(int argc, char **argv) {
    Channel<string> ch(5);

    vector<thread> tasks;

    tasks.emplace_back(writeRoutine, ref(ch), 500, 1, 8);
    tasks.emplace_back(writeRoutine, ref(ch), 300, 2, 3);
    tasks.emplace_back(writeRoutine, ref(ch), 250, 3, 2);

    tasks.emplace_back(readRoutine, ref(ch));

    for (thread &t : tasks)
        t.join();

    return 0;
}
