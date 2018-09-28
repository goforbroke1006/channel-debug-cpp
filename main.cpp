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
        ch >> s;
        cout << s << endl;
    }
}

int main(int argc, char **argv) {
    Channel<string> ch(50000);

    vector<thread> tasks;

    int limit1 = 8000;
    int limit2 = 3000;
    int limit3 = 2000;
    int limit4 = 9000;
    int limit5 = 3000;

    tasks.emplace_back(writeRoutine, ref(ch), /*5*/0, 1, limit1);
    tasks.emplace_back(writeRoutine, ref(ch), /*3*/0, 2, limit2);
    tasks.emplace_back(writeRoutine, ref(ch), /*2*/0, 3, limit3);
    tasks.emplace_back(writeRoutine, ref(ch), /*4*/0, 4, limit4);
    tasks.emplace_back(writeRoutine, ref(ch), /*8*/0, 5, limit5);

    int bitesCount = limit1 + limit2 + limit3 + limit4 + limit5;
    cout << "Bites count: " << bitesCount << endl;

    tasks.emplace_back(readRoutine, ref(ch), bitesCount);

    for (thread &t : tasks)
        t.join();

    return 0;
}
