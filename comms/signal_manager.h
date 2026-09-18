#ifndef _SIGNAL_MANAGER_
#define _SIGNAL_MANAGER_

#include <csignal>
#include <iostream>

/**
 * @brief Very simple signal mask handler.
 *        Makes it easy to add signals to a single signal handler function
 */
class SignalManager {
    sigset_t sigmask_;
    sigset_t origmask_;
  public:

    /**
     * @brief Create a new SignalManager
     *
     * @param signal_handler function of type int -> void to be called
     * @param signum list of signal numbers to be regestered with this function
     *
     * @details ex:
     *
     *          void signalHandler(int signum) {
     *              ...
     *          }
     *
     *          SignalManager sig_manager {signalHandler, SIGINT};
     *
     *          Now, whenever SIGINT is raised, signalHandler(SIGINT) is called.
     */
    template<typename ... signums>
    SignalManager(sig_t signal_handler, signums... signum) {
        sigfillset(&sigmask_);
        (std::signal(signum, signal_handler), ...);
        (sigdelset(&sigmask_, signum), ...);
        sigprocmask(SIG_SETMASK, &sigmask_, &origmask_);
    }

    /**
     * @brief Resets the signal mask to what it initially was
     */
    ~SignalManager() {
        sigprocmask(SIG_SETMASK, &origmask_, nullptr);
    }
};

#endif
