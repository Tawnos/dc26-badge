#pragma once
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <array>

using namespace std::chrono_literals;

template <class TMessage, size_t NElements>
class CommandQueue
{
public:
   CommandQueue(std::chrono::milliseconds pollTimeout = 1000ms)
      : pollTimeout(pollTimeout)
   {}

   TMessage* push(TMessage* msg)
   {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      currentItem++;
      if (currentItem < NElements)
      {
         auto msgDest = &queueStorage[currentItem];
         memcpy((void*)msgDest, msg, sizeof(TMessage));
         queue.push(msgDest);
         return msgDest;
      }
      currentItem--;
      return nullptr;
   }

   TMessage* pop(std::stop_token stoken)
   {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      if (currentItem == -1)
         queue_condition.wait_for(queue_lock, pollTimeout);

      if (queue.empty())
         return nullptr;

      auto m = queue.front();
      queue.pop();
      return m;
   }

private:
   std::chrono::milliseconds pollTimeout;

   int currentItem{ -1 };
   std::queue<TMessage*> queue{};
   std::array<TMessage, NElements> queueStorage{};
   std::mutex queue_mutex;
   std::unique_lock<std::mutex> queue_lock;
   std::condition_variable queue_condition;
};