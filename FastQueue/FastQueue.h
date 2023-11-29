//
// Created by nitish on 29/11/23.
//

#ifndef DECS_AUTOGRADER_FASTQUEUE_H
#define DECS_AUTOGRADER_FASTQUEUE_H

#include <iostream>
#include<list>
#include <unordered_map>
using namespace  std;

typedef list<uint32_t> li;

class FastQueue {

    li que;
    unordered_map <uint32_t , li::iterator> mp;

public:

    void pop();
    void push(uint32_t key);
    uint32_t front();
    int get_index(uint32_t key);
    bool empty();

};

#endif //DECS_AUTOGRADER_FASTQUEUE_H
