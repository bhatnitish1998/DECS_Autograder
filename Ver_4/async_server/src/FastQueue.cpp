//
// Created by nitish on 29/11/23.
//

#include "FastQueue.h"

uint32_t FastQueue::front()
{
    return que.front();
}

bool FastQueue::empty()
{
    return que.empty();
}

void FastQueue::push(uint32_t key)
{
    que.push_back(key);
    mp[key] = prev(que.end());
}

void FastQueue::pop()
{
    uint32_t key = que.front();
    que.pop_front();
    mp.erase(mp.find(key));
}

int FastQueue::get_index(uint32_t key)
{
    auto it = mp.find(key);
    if (it == mp.end())
        return -1;
    else
    {
        auto element_pointer = it->second;
        int position = std::distance(que.begin(), element_pointer);
        return position;
    }
}

int FastQueue::size()
{
    return que.size();
}