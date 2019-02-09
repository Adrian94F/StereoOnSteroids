#ifndef CALIB_CAM_TASKMANAGER_HPP
#define CALIB_CAM_TASKMANAGER_HPP

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/shared_memory_object.hpp>

#define TASK_MANAGER_SHARED_MEMORY "shm"

using namespace boost::interprocess;

class TaskManager
{
public:
    TaskManager();
    void start(int nOfSlaves = 2);
private:
    void masterTask();
    void slaveTask(int pid);
    int numberOfSlaves_;
};


#endif //CALIB_CAM_TASKMANAGER_HPP
