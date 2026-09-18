#include "file_descriptor_manager.h"

FileDescriptorManager::FileDescriptorManager(int fd) {
    epoll_fd_ = epoll_create1(0);
    if (epoll_fd_ < 0)
        throw FileDescriptorManagerException("Error creating epoll file descriptor");

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;

    event.data.fd = fd;
    int status = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
    if (status < 0)
        throw FileDescriptorManagerException("Error adding file descriptor to epoll_fd");

    fds_.push_back(fd);
    ready_events_ = new struct epoll_event[1];
}

FileDescriptorManager::~FileDescriptorManager() {
    delete[] ready_events_;
    if (close(epoll_fd_) < 0)
        std::cerr << "Error closing file descriptor manager" << std::endl;
}

void FileDescriptorManager::addFileDescriptor(int fd) {
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = EPOLLIN;

    event.data.fd = fd;
    int status = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &event);
    if (status < 0)
        throw FileDescriptorManagerException("Error adding file descriptor to epoll_fd");

    fds_.push_back(fd);
    delete[] ready_events_;
    ready_events_ = new struct epoll_event[fds_.size()];
}

int FileDescriptorManager::mwait(int ms_timeout) {

    num_ready_events_ = epoll_wait(epoll_fd_, ready_events_, fds_.size(), ms_timeout);

    return num_ready_events_;
}

int FileDescriptorManager::wait(int s_timeout) {
    return mwait(s_timeout * 1000);
}

bool FileDescriptorManager::areBytesWaiting(int fd) {
    for (int i = 0; i < num_ready_events_; i++)
        if (ready_events_[i].data.fd == fd)
            return true;
    return false;
}
