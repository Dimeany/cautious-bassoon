#include <chrono>
#include <iomanip>
#include <fstream>
#include <poll.h>
#include <signal.h>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <socket.h>
#include "../util/environment_util.h"
#include "version.h"

#include <syslog.h>
#include <systemd/sd-journal.h>

// Commands time out after 10 seconds
constexpr int command_timeout_ms = 10000;
constexpr int max_response_size = 1280;

// An exception type
struct CommandServiceException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


// Reads all data from a file descriptor, or times out.
std::string
read_until_eof(int fd, int timeout_ms) {
    // Get start time, for timeout
    auto start_time = std::chrono::system_clock::now();

    // Contains all bytes read from file
    std::stringstream output_stream;

    // Poll for input available to read
    struct pollfd poll_fd {
        .fd = fd,
        .events = POLLIN,
        .revents = 0,
    };

    // Loop until EOF is reached or we time out
    int poll_result = 0;
    while ((poll_result = poll(&poll_fd, 1, timeout_ms))) {
        // Check for error
        if (poll_result == -1) {
            throw CommandServiceException(std::string("poll failed in read_from_child: ") + std::strerror(errno) + "\nOutput so far:\n" + output_stream.str());
        }

        // Clear revents (not used, but good practice)
        poll_fd.revents = 0;

        // Read from file
        std::string buffer;
        buffer.resize(1024);
        int bytes_read = read(fd, buffer.data(), buffer.size());
        if (bytes_read == 0) {
            // Reached EOF
            break;
        }
        if (bytes_read < 0) {
            // Read error
            throw CommandServiceException(std::string("read failed: ") + std::strerror(errno) + "\nOutput so far:\n" + output_stream.str());
        }

        // Append bytes to stringstream
        buffer.resize(bytes_read);
        output_stream << buffer;

        // Check for command timeout
        auto now = std::chrono::system_clock::now();
        auto duration = now - start_time;
        if (timeout_ms < std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()) {
            // Kind of a hack, to hit the if statement following this loop
            poll_result = 0;
            break;
        }
    }

    // Command output never become ready for reading / command timed out.
    if (poll_result == 0) {
        throw CommandServiceException(std::string("Command timed out.\nOutput so far:\n") + output_stream.str());
    }

    // Success, return command output
    return output_stream.str();
}


// Construct success and error responses
std::string success_prefix = "ack-ok\n";
std::string error_prefix = "error\n";


// Returns the status from waitpid if process could be joined,
// otherwise returns the empty optional
std::optional<int> join_process(pid_t pid) {
    (void)pid;

    // Wait for SIGCHLD
    sigset_t signal_set;
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGCHLD);

    siginfo_t info {};

    struct timespec timeout {
        .tv_sec = 1,
        .tv_nsec = 0,
    };

    int result = sigtimedwait(&signal_set, &info, &timeout);
    if (result != SIGCHLD) {
        return {};
    }

    int status = 0;
    result = waitpid(pid, &status, WNOHANG);
    if (result < 0) {
        return {};
    }
    return status;
}

// Execute a command, and return its exit status, standard out, and
// standard error
std::string run_command(std::string command_string) {
    // Create a pipe for child to write its standard out and standard
    // error into, and for the parent to read from
    int pipe_fd[2];
    pipe(pipe_fd);

    // Fork a child process
    int pid = fork();
    if (pid == 0) {
        // Child process

        // Close "read" end of pipe.
        close(pipe_fd[0]);

        // Send standard out and stndard error to "write" end of pipe.
        dup2(pipe_fd[1], 1);
        dup2(pipe_fd[1], 2);

        // Close the "write" end of the pipe, now that it is referred
        // to by standard out and standard error.
        close(pipe_fd[1]);

        // Run the command!
        execl("/bin/bash", "bash", "-c", command_string.c_str(), nullptr);

        // This code should not be reached.  If it is, output the
        // error and exit the child process with a failure status.
        std::cout << "Exec failed: " << std::strerror(errno) << std::endl;
        exit(1);
    }
    if (pid < 0) {
        return error_prefix + std::string("fork failed: ") + std::strerror(errno);
    }

    // Parent process

    // Close "write" end of the pipe.
    close(pipe_fd[1]);

    // Track whether an error occurred reading from the child process,
    // joining the child process, or as indicated by its exit status.
    bool error_occurred = false;

    // Read from the "read" end of the pipe.
    std::string command_output;
    auto current_time = time(nullptr);

    std::stringstream output_stream;
    output_stream << "Date: " << current_time << "\n";
    output_stream << "Command: " << command_string << "\n";
    try {
        command_output = read_until_eof(pipe_fd[0], command_timeout_ms);
    }
    catch (CommandServiceException const& ex) {
        // Timeout or other error occurred reading from process.  Terminate it.
        error_occurred = true;
        output_stream << ex.what();
        kill(pid, SIGTERM);
    }

    // Close "read" end of pipe.  Done with it!
    close(pipe_fd[0]);

    // Try to join the process.
    std::optional<int> status = join_process(pid);

    // If the process did not join, kill it and try again.
    if (!status) {
        kill(pid, SIGKILL);
        status = join_process(pid);
    }

    if (!status) {
        // Process still did not join.  (Bad.  Log an error.)
        output_stream << "Could not join process!  Unknown exit status.";
        std::cout << "Could not join process with pid: " << pid << std::endl;
        error_occurred = true;
    }
    else if (WIFEXITED(*status)) {
        // Process exited with an exit status
        auto exit_status = WEXITSTATUS(*status);
        output_stream << "Exit status: " << exit_status;
        if (exit_status != 0) {
            // If exit status was not 0, consider this an error
            error_occurred = true;
        }
    }
    else if (WIFSIGNALED(*status)) {
        // Process exited due to a signal
        output_stream << "Exit signal: " << strsignal(WTERMSIG(*status));
        error_occurred = true;
    }
    else {
        // Waitpid returned something unexpected...
        output_stream << "Unexpected status from waitpid: " << *status;
        std::cout << "Could not join process with pid: " << pid << std::endl;
        error_occurred = true;
    }

    output_stream << "\n";

    std::string output_meta = (error_occurred ? error_prefix : success_prefix) + output_stream.str();

    // Try to append command output to output metadata string.
    std::string result = output_meta + "Output:\n" + command_output;

    // If response is too long, write to a file and instead append
    // file name to output metadata string
    if (result.size() > max_response_size) {
        // time format string copied from udp_capture
        char const* time_format = "%Y-%j-%H-%M-%S";
        auto now_time_t = std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now()
        );

        auto file_name = (
            std::stringstream {}
            << "cmd_"
            << std::put_time(std::gmtime(&now_time_t), time_format)
            << ".bin"
        ).str();

        // With append mode, even if two long commands are run during
        // the same second, both get saved
        std::ofstream file(file_name, file.app);
        file << result;

        // Just take the first 1000 bytes
        result = (
            std::stringstream {}
            << output_meta
            << "Output first bytes:\n"
            << command_output.substr(0, 1000) << "\n"
            << "Full output written to file:\n"
            << file_name << "\n"
        ).str();
    }

    int ret = sd_journal_send(
        "MESSAGE=Command executed\nCommand: %s\nResponse: %s",
        command_string.c_str(),
        result.c_str(),
        "PRIORITY=%i", LOG_INFO,
        NULL
    );
    if (ret < 0) {
        std::cerr << "Failed to send log to systemd journal: " << std::strerror(errno) << std::endl;
    }
    return result;
}

// Adds an option to use UDP framing according to the COMMS standards
bool parse_cmd_opts(int argc, char *argv[]) {
    // Add an option for using the smallsat UDP framing
    bool framing = false;

    // Parse for the framing bool
    int opt = 0;
    while ((opt = getopt(argc, argv, "fh")) != -1) {
        if (opt == 'f') framing = true;
        else if (opt == 'h') { std::cout << "Add -f to use SSRL's UDP framing!\n"; exit(EXIT_SUCCESS); }
    }

    return framing;
}


int main(int argc, char *argv[]) {
    bool framing = parse_cmd_opts(argc, argv);

    openlog("CommandService", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "Nebula version: %s", NEBULA_VERSION);

    // Block SIGCHLD so that we can wait for it with timeouts
    sigset_t mask;
    sigset_t orig_mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &mask, &orig_mask) < 0) {
        std::cout << "Could not block SIGCHLD: " << std::strerror(errno) << std::endl;
    }

    // Create a socket and listen for packets
    auto cmd_socket_address = udp::SocketAddress::create("0.0.0.0", atoi(util::getenv_required("CMD_COMMAND_PORT")));
    auto socket = udp::Socket(cmd_socket_address);
    std::string filename = "cmdlog.txt";
    while (true) {
        // Get a bash command to run
        udp::SocketAddress sender {};
        auto opt_message = socket.receive(sender);
        if (!opt_message) {
            continue;
        }

        // Here we differentiate between framing and no framing
        if (framing) {
            std::vector<char>& msg = *opt_message;
            
            // Decode the message into a real packet
            udp::Packet pkt = udp::Packet::deserialize(msg);

            // Run the command found within the packet
            auto command_string = std::string(pkt.payload.data(), pkt.payload.size());
            auto response = run_command(command_string);

            // Create a response packet with source and destination flipped
            std::vector<char> response_pkt = udp::Packet::create_from_buffer(
                response, 
                pkt.header.source_port,
                pkt.header.destination_port,
                0,
                0
            ).serialize();
            socket.send(response_pkt, sender);
            
        } else {
            // Run the command, and send the response
            auto command_string = std::string(opt_message->data(), opt_message->size());
            auto response = run_command(command_string);
            socket.send(response, sender);
        }

    }
}

