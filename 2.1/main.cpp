#include <iostream>
#include <vector>
#include <array>
#include <cassert>
#include <list>

using namespace std;

template<class Iterator>
void print(Iterator beg, Iterator end) {
    while (beg != end) {
        cout << *(beg++) << " ";
    }
}

void vectorTest() {
    vector<int> a = {1, 2, 3};
    vector<int>::reverse_iterator x = a.rbegin();
    vector<int>::const_iterator z = a.cbegin();
    vector<int>::const_reverse_iterator w = a.crbegin();

    print(a.begin(), a.end());
}

void arrayTest() {
    std::array<int, 16> small_array{};
    assert(small_array.size() == 1 << 4);
    for (int i = 0; i < small_array.size(); ++i) {
        small_array[i] = i * i ^ 2 + 1;
    }
    print(small_array.begin(), small_array.end());
}

void listTest() {
    list<int> l;
    l.push_back(1);
    l.push_back(2);
    l.push_front(4);
    l.push_front(8);

    print(l.begin(), l.end()); cout << endl;
    l.push_front(1);
    l.remove(1);

    print(l.begin(), l.end());
}

int main() {
//    vectorTest();
//    arrayTest();
    listTest();
}