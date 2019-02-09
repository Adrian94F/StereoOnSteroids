#include "TaskManager.hpp"

#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

TaskManager::TaskManager()
{
    shared_memory_object shm_obj(create_only, TASK_MANAGER_SHARED_MEMORY, read_only);
}

void TaskManager::start(int nOfSlaves)
{
    numberOfSlaves_ = nOfSlaves;
    shared_memory_object shm_obj(open_only, TASK_MANAGER_SHARED_MEMORY, read_write);

    int pid, status;

    for (int slave = 0; slave < numberOfSlaves_; slave++)
    {
        if ((pid = fork()) == 0)
        {
            slaveTask(pid);
            exit(0);
        }
    }

    masterTask();

    for (int slave = 0; slave < numberOfSlaves_; slave++)
    {
        pid = wait(&status);
        std::cout << "Slave " << pid << " ended his job and returned " << WEXITSTATUS(status) << std::endl;
    }

    shared_memory_object::remove(TASK_MANAGER_SHARED_MEMORY);
}

void TaskManager::masterTask()
{

    std::cout << "Master ready!" << std::endl;
}

void TaskManager::slaveTask(int pid)
{

    std::cout << "Slave " << pid << " ready!" << std::endl;
}