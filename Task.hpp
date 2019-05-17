struct Task
{
    enum
    {
        ETaskStatus_Wait,
        ETaskStatus_TODO,
        ETaskStatus_Done,
        ETaskStatus_Abort
    } typedef ETaskStatus;
    ETaskStatus status;
};