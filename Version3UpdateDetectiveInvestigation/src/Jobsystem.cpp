#include "Jobsystem.hpp"

Jobsystem::Jobsystem(size_t numThreads)
{
  // Check if the user put more than the available threads

  if (numThreads > std::thread::hardware_concurrency()) numThreads = std::thread::hardware_concurrency();

  // Always will be a thread (main)

  if (numThreads == 0) numThreads = 1;

  for (size_t i = 0; i < numThreads; ++i) {
      workers.emplace_back([this]() { WorkerLoop(); });
  }
}

Jobsystem::~Jobsystem()
{
  // Stops all the threads and notify all to wake

  stop.store(true);
  cv.notify_all();
  
  for (auto& t : workers)
    if (t.joinable()) t.join();
}

void Jobsystem::Enqueue(Job job)
{
  // Block to free the lock guard
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    jobQueue.push([this, job]() {
      activeJobs.fetch_add(1, std::memory_order_relaxed);
      try {
        job();
      }
      catch (const std::exception& e) {
        printf("Error on thread job execution: %s\n", e.what());
      }
      catch (...) {
        printf("Unknown error on thread job execution\n");
      }
      activeJobs.fetch_sub(1, std::memory_order_relaxed);
    });
  }

  // Notify one thread to catch a job

  cv.notify_one();
}

void Jobsystem::WaitAll()
{
  // Loop to wait, works but :/

  for (;;) {

    // Block for lock_guard

    {
      std::lock_guard<std::mutex> lock(queueMutex);
      if (jobQueue.empty() && activeJobs.load(std::memory_order_relaxed) == 0)
        return;
    }

    // If the job queue is not empty or there is some thread working wait 1 ms

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

void Jobsystem::WorkerLoop()
{
  while (!stop.load()) {
    Job job;
    {
      std::unique_lock<std::mutex> lock(queueMutex);

      // Wait until need to work (stop = true or jobqueue has jobs)

      cv.wait(lock, [this]() { return stop.load() || !jobQueue.empty(); });
      if (stop.load() && jobQueue.empty()) return;

      // Take the job

      if (!jobQueue.empty()) {
        job = std::move(jobQueue.front());
        jobQueue.pop();
      }
    }
    if (job) job(); // Run job on this thread outside the mutex
  }
}
