#include "backend.h"

namespace comms {
    namespace backend {
        class PPP : public Backend {
            std::shared_ptr<Backend> inner;

            // Buffer holding input read from inner backend.  Refilled
            // when empty.
            size_t index = 0;
            std::vector<char> raw_bytes;

            // State machine of PPP backend
            std::vector<char> partial_frame;
            enum class State {
                // Waiting to see FLAG to begin PPP packet
                WaitingForFlag,

                // Waiting to see FLAG, and saw ESC
                WaitingForFlagEsc,

                // Saw FLAG, have not read any bytes yet.
                ReadingFirstByte,

                // Reading bytes :)
                Reading,

                // Reading bytes and saw ESC
                ReadingEsc,
            } state;

        public:
            static constexpr char FLAG {0x7e};
            static constexpr char ESC {0x7d};

            PPP(std::shared_ptr<Backend> inner_);


            std::vector<char> finalize_frame();

            int getFd() override;
            size_t write(std::span<char const> data) override;
            std::vector<char> read() override;
            bool bytesAvailable() override;
        };
    }
}
