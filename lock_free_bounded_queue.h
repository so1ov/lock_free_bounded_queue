#include <atomic>
#include <thread>
#include <array>

template<typename T, T Default, int Size>
class LockFreeBoundedQueue {
    public:
        LockFreeBoundedQueue() {
            for (T& t : m_arr) {
                t = Default;
            }

            m_read.store(0, std::memory_order_release);
            m_write.store(0, std::memory_order_release);
        }

        void push(const T& el) {
            while (m_read.load(std::memory_order_acquire) % Size == ((m_write.load(std::memory_order_relaxed) + 1) % Size)) {
                ;
            }

            m_arr[m_write.load(std::memory_order_relaxed) % Size] = el;
            m_write.fetch_add(1, std::memory_order_release);
        }

        T pop() {
            while (m_arr[m_read.load(std::memory_order_relaxed) % Size] == Default) {
                ;
            }

            T ret = m_arr[m_read % Size];
            m_arr[m_read % Size] = Default;

            m_read.fetch_add(1, std::memory_order_release);

            return ret;
        }

    private:
        std::array<T, Size> m_arr;
        std::atomic<int> m_read;
        std::atomic<int> m_write;
};