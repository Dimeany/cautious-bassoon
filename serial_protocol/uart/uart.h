#include <cstring>
#include <string>
#include <span>
#include <vector>

class UARTException : public std::exception
{
private:
    std::string m_error;

public:
    UARTException(const std::string &error_message)
    {
        if (errno > 0)
        {
            m_error = error_message + ", errno: " + std::to_string(errno);
        }
        else
        {
            m_error = error_message;
        }
    }
    const char *what() const noexcept override
    {
        return m_error.c_str();
    }
};

class UART
{
private:
    int fd;

    void SetSettings(int baud_rate);
public:

    void UpdateVMIN(size_t new_vmin);
    void UpdateVTIME(size_t new_vtime);

    UART();
    ~UART();
    int Open(const char *port, int baud_rate);
    std::vector<std::byte> Read();

    void Read(void *dest, size_t num_bytes);
    size_t ReadOnly(void *dest, size_t num_bytes);
    std::string ReadLine(std::string line_end_sequence);

    int Write(std::span<char const> data_to_send);

    int get_fd();
};
