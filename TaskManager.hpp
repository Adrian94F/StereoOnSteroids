#ifndef CALIB_CAM_TASKMANAGER_HPP
#define CALIB_CAM_TASKMANAGER_HPP

#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/anonymous_shared_memory.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/thread/mutex.hpp>

#include "ImageCorrection.hpp"

using namespace boost::interprocess;

#define MAT cv::Mat
#define MUTEX boost::mutex

typedef boost::interprocess::allocator<MAT, managed_shared_memory::segment_manager>  ShmemMatAllocator;
typedef std::vector<MAT, ShmemMatAllocator> SharedMatVector;
typedef boost::interprocess::allocator<MUTEX, managed_shared_memory::segment_manager>  ShmemMutexAllocator;
typedef std::vector<MUTEX*, ShmemMutexAllocator> SharedMutexVector;

class TaskManager
{
public:
    TaskManager();
    void start();
private:
    void masterTask();
    void slaveTask(int);
    void prepareSharedData();
    void removeSegment();
    uint numberOfSlaves_;
    uint numberOfTasks_;
    managed_shared_memory segment_;
};


#endif //CALIB_CAM_TASKMANAGER_HPP
