#include <atomic>
#include <thread>
#include <array>

template<typename T, T Default>
class LockFreeBoundedQueue {
    public:
        LockFreeBoundedQueue(int _m_size)
            :   m_size{_m_size} {
            m_arr = new T[m_size];

            for (int i = 0; i < m_size; i++) {
                m_arr[i] = Default;
            }

            m_read.store(0, std::memory_order_release);
            m_write.store(0, std::memory_order_release);
        }

        void push(const T& el) {
            while (m_read.load(std::memory_order_acquire) % m_size == ((m_write.load(std::memory_order_relaxed) + 1) % m_size)) {
                ;
            }

            m_arr[m_write.load(std::memory_order_relaxed) % m_size] = el;
            m_write.fetch_add(1, std::memory_order_release);
        }

        T pop() {
            while (m_arr[m_read.load(std::memory_order_relaxed) % m_size] == Default) {
                ;
            }

            T ret = m_arr[m_read % m_size];
            m_arr[m_read % m_size] = Default;

            m_read.fetch_add(1, std::memory_order_release);

            return ret;
        }

    private:
        T* m_arr;
        int m_size;
        std::atomic<int> m_read;
        std::atomic<int> m_write;
};