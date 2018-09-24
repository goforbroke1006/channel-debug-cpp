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
