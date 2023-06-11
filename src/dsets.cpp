/* Your code here! */
#include "dsets.h"


void DisjointSets::addelements(int num) {
    while (num > 0) {
        elements_.push_back(-1);
        --num;
    }
}

int DisjointSets::find(int elem) {
    if (elements_.at(elem) < 0) {
        return elem;
    } else {
        int find_ = find(elements_.at(elem));
        elements_.at(elem) = find_;
        return find_;
    }
}

void DisjointSets::setunion(int a, int b) {
    // int a_index = a;
    // int b_index = b;
    // if (elements_.at(a) >= 0) {
    //     int a_index = find(a);
    // }
    // if (elements_.at(b) >= 0) {
    //     int b_index = find(b);
    // }
    while (elements_.at(a) >= 0) {
        a = elements_.at(a);
    }
    while (elements_.at(b) >= 0) {
        b = elements_.at(b);
    }
    int sz = elements_.at(a) + elements_.at(b);
    if (elements_.at(b) < elements_.at(a)) {
        elements_.at(a) = b;
        elements_.at(b) = sz;
    } else {
        elements_.at(b) = a;
        elements_.at(a) = sz;
    }
    // std::cout << a << b << b_index << '\n';
}

int DisjointSets::size(int elem) {
    while (elements_.at(elem) >= 0) {
        elem = elements_.at(elem);
    }
    return elements_.at(elem) * -1;
}
