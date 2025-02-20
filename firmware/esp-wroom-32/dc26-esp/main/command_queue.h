#pragma once
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <array>
#include <cassert>

using namespace std::chrono_literals;

template <class TMessage>
class CommandQueue {
public:
   CommandQueue(std::chrono::milliseconds pollTimeout = 1000ms)
      : pollTimeout(pollTimeout)
   {}
   virtual const TMessage* push(TMessage* msg) = 0;
   virtual TMessage* pop() = 0;

protected:
   std::chrono::milliseconds pollTimeout;

};

template <class TMessage, size_t NElements>
class CommandQueue : CommandQueue<TMessage>
{
public:
   using CommandQueue<TMessage>::CommandQueue();

   const TMessage* push(TMessage* msg) override
   {
      std::unique_lock<std::mutex> queue_lock(queue_mutex, std::defer_lock);
      //while (!stoken.stop_requested())
      while (true)
      {
         queue_lock.lock();
         //queue_lock.try_lock_for(pollTimeout)
         currentItem++;
         if (currentItem < NElements)
         {
            auto msgDest = &queueStorage[currentItem];
            memcpy((void*)msgDest, msg, sizeof(TMessage));
            queue_condition.notify_one();
            return msgDest;
         }
         currentItem--;

         // before continuing the loop, temporarily unlock to allow a pop to occur
         queue_lock.unlock();
      }
      return nullptr;
   }

   TMessage* pop() override
   {
      std::unique_lock<std::mutex> queue_lock(queue_mutex);
      if (currentItem == -1)
         queue_condition.wait_for(queue_lock, pollTimeout);

      if (currentItem == -1)
            return nullptr;

      auto m = new TMessage{};
      memcpy((void*)m, &queueStorage[currentItem], sizeof(TMessage));
      currentItem--;
      return m;
   }

private:

   int currentItem{ -1 };
   std::array<TMessage, NElements> queueStorage{};
   std::mutex queue_mutex;
   std::unique_lock<std::mutex> queue_lock;
   std::condition_variable queue_condition;
};