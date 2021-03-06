//
// Created by goforbroke on 24.09.18.
//

#ifndef CHANNEL_DEBUG_CHANNEL_H
#define CHANNEL_DEBUG_CHANNEL_H

#include <queue>
#include <condition_variable>

using namespace std;

template<class T>
class channel;

template<class T>
void operator<<(T &val, channel<T> &chan);

template<class T>
class channel {
private:
    size_t capacity;
    queue<T> buffer;
    mutex buffer_mutex;
    condition_variable cv_empty;
    condition_variable cv_full;
public:
    explicit channel(size_t c) : capacity(c) {}

    /**
     * write to channel
     */
    inline void operator<<(const T obj) {
        unique_lock<mutex> lock(buffer_mutex);
        cv_full.wait(lock, [this] { return getLength() < getCapacity(); });
        buffer.push(obj);
        cv_empty.notify_one();
    }

    /**
     * read from channel
     */
    /*inline void operator>>(T &obj) {
        unique_lock<mutex> lock(buffer_mutex);
        cv_empty.wait(lock, [this] { return getLength() > 0; });
        obj = buffer.front();
        buffer.pop();
        cv_full.notify_one();
    }*/

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

private:
    /**
     * read from channel
     */
    void copyTo(T &obj) {
        unique_lock<mutex> lock(buffer_mutex);
        cv_empty.wait(lock, [this] { return getLength() > 0; });
        obj = buffer.front();
        buffer.pop();
        cv_full.notify_one();
    }

    template<class X>
    friend void operator<<(X &val, channel<X> &chan);
};

template<class T>
void operator<<(T &val, channel<T> &chan) {
    chan.copyTo(val);
}

#endif //CHANNEL_DEBUG_CHANNEL_H
