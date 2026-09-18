#ifndef _FILE_DESCRIPTOR_MANAGER_
#define _FILE_DESCRIPTOR_MANAGER_

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/epoll.h>

class FileDescriptorManagerException : public std::exception {
    std::string what_;
  public:
    FileDescriptorManagerException(std::string what) : what_(what) {}
    const char* what() const throw() {
        return what_.c_str();
    }
};

class FileDescriptorManager {
    std::vector<int> fds_;

    struct epoll_event* ready_events_;
    ssize_t num_ready_events_;

    int epoll_fd_;

  public:

    /**
     * @brief Constructor for FileDescriptorManager
     *
     * @param fd initial file descriptor to be created around
     *
     * @return FileDescriptorManager
     *
     * @throws FileDescriptorManagerException
     */
    FileDescriptorManager(int fd);

    /**
     * @brief Destructor for FileDescriptorManager
     */
    ~FileDescriptorManager();

    /**
     * @brief Adds a file descriptor to the epoll file
     *
     * @param fd file descriptor to be added
     *
     * @throws FileDescriptorManagerException
     */
    void addFileDescriptor(int fd);

    /**
     * @brief Waits until bytes are ready in one of the managed file descriptors or for timeout
     *
     * @param timeout milliseconds before breaking
     *
     * @return number of file descriptors that have bytes waiting
     */
    int mwait(int ms_timeout = -1);

    /**
     * @brief Waits until bytes are ready in one of the managed file descriptors or for timeout
     *
     * @param timeout seconds before breaking
     *
     * @return number of file descriptors that have bytes waiting
     */
    int wait(int s_timeout = -1);

    /**
     * @brief Determines whether or not bytes are waiting in the specified file descriptor
     *        Must be preceded by wait() or mwait() for most accurate results
     *
     * @param fd file descriptor to check if waiting
     *
     * @return true if bytes are waiting in file, false otherwise
     */
    bool areBytesWaiting(int fd);
};

#endif
