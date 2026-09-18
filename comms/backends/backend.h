#ifndef _BACKEND_H_
#define _BACKEND_H_

#include <span>
#include <vector>
#include <memory>

namespace comms {
    namespace backend {

        class Backend {
          public:

            Backend() {};
            virtual ~Backend() {};

            // Return the file descriptor to poll on for this backend
            virtual int getFd() = 0;

            /**
             * @brief Write an entire span to the backend implementation
             *
             * @param data An std::span<char const> with data to be written
             *
             * @return size_t amount of data written
             */
            virtual size_t write(std::span<char const> data) = 0;

            /**
             * @brief Read an indeterminate amount from the backend implementation
             *
             * @return std::vector<char> fully filled with data read
             */
            virtual std::vector<char> read() = 0;

            /**
             * @brief Determines if bytes are certainly available in backend
             *
             * @return bool true if bytes are certainly available. false otherwise. False does not necessarily imply bytes are not available.
             */
            virtual bool bytesAvailable() = 0;
        };
    }
}

#endif
