/**
*
* @file Jobsystem.hpp
*
* @author Vicente Bellot Vila
* @author Juan Jose Valenzuela Gomez
*
* @brief Threading system
*
*/

#ifndef __JOBSYSTEM_HTML__
#define __JOBSYSTEM_HTML__ 1

#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <future>

/**
*
* @class Jobsystem
*
* @brief This class controls all the threading behaviour
*
*/
class Jobsystem {
public:
	using Job = std::function<void()>;																				/// To avoid writing std::function<void()> a lot

	/**
	* 
	* @brief The constructor for the class
	* 
	* @param numThreads A size_t that indicates the max number of threads available for the hardware
	* 
	*/ 
	Jobsystem(size_t numThreads = std::thread::hardware_concurrency());

	/**
	*
	* @brief The destructor for the class
	*
	*/
	~Jobsystem();

	/**
	*
	* @brief Adds a job (std::function<void()>) to the queue to be executed by a thread
	*
	* @param job The job to enqueue
	*
	*/
	void Enqueue(Job job);

	/**
	*
	* @brief Waits all jobs on the queue to end
	*
	*/
	void WaitAll();

	/**
	*
	* @brief Enqueue a job that returns a value and provides a std::future to obtain the result
	*
	* @param func The function to be executed
	* @param args The function agrguments
	* @return std::future<std::invoke_result_t<F, Args...>> Un objeto future para recuperar el resultado
	*
	*/
	template<typename F, typename... Args>
	auto EnqueueTask(F&& func, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
		using ReturnType = std::invoke_result_t<F, Args...>;

		// Wrap callable in a packaged_task

		auto task = std::make_shared<std::packaged_task<ReturnType()>>(
			std::bind(std::forward<F>(func), std::forward<Args>(args)...)
		);

		std::future<ReturnType> future = task->get_future();

		// Enqueue a lambda that executes the packaged_task

		Enqueue([task]() { (*task)(); });

		return future;
	}

private:
	/**
	*
	* @brief Main loop to execute each job thread
	*
	*/
	void WorkerLoop();

	std::vector<std::thread> workers;																					/// The threads vector
	std::queue<Job> jobQueue;																									/// The pending job queue

	std::mutex queueMutex;																										/// Mutex that protects queue
	std::condition_variable cv;																								/// Condition variable to wake the threads
	std::atomic<bool> stop{ false };																					/// To stop all threads
	std::atomic<int> activeJobs{ 0 };																					/// Counter of jobs being executed
};
#endif