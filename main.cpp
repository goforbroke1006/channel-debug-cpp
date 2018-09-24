#include <iostream>
#include <thread>
#include <unistd.h>

#include "channel.h"

using namespace std;

#define SECOND_SLEEP 1000000

void writeRoutine(Channel<string> &ch, __useconds_t delay, int index, unsigned int fakeValuesCount) {
    for (int i = 0; i < fakeValuesCount; ++i) {
        ch << ("hello " + to_string(index));
        usleep(delay);
    }
}

void readRoutine(Channel<string> &ch, unsigned int waitJobsCount) {
    string s;
    for (int i = 0; i < waitJobsCount; ++i) {
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

    int limit1 = 8;
    int limit2 = 3;
    int limit3 = 2;

    tasks.emplace_back(writeRoutine, ref(ch), 500, 1, limit1);
    tasks.emplace_back(writeRoutine, ref(ch), 300, 2, limit2);
    tasks.emplace_back(writeRoutine, ref(ch), 250, 3, limit3);

    tasks.emplace_back(readRoutine, ref(ch),
                       limit1 + limit2 + limit3);

    for (thread &t : tasks)
        t.join();

    return 0;
}
