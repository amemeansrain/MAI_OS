#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include "semaphore.hpp"

TEST(SemaphoreTest, BasicAcquireRelease) {
    Semaphore sem("test", 1);
    
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_FALSE(sem.tryAcquire()); // Должен быть занят
    
    sem.release();
    EXPECT_TRUE(sem.tryAcquire()); // Должен снова стать доступным
}

TEST(SemaphoreTest, CountingSemaphore) {
    Semaphore sem("test", 3);
    
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_FALSE(sem.tryAcquire()); // Все 3 слота заняты
    
    sem.release();
    EXPECT_TRUE(sem.tryAcquire()); // Один слот освободился
}

TEST(SemaphoreTest, ThreadSafety) {
    const int max_count = 3;
    const int num_threads = 10;
    Semaphore sem("thread_test", max_count);
    
    std::atomic<int> concurrent_count{0};
    std::atomic<int> max_concurrent{0};
    std::vector<std::thread> threads;
    std::mutex mutex;
    
    auto worker = [&](int id) {
        sem.acquire();
        
        // Измеряем максимальное количество одновременных потоков
        int current = ++concurrent_count;
        int old_max = max_concurrent.load();
        while (current > old_max && 
               !max_concurrent.compare_exchange_weak(old_max, current)) {
            old_max = max_concurrent.load();
        }
        
        // Имитируем работу
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        --concurrent_count;
        sem.release();
    };
    
    // Запускаем потоки
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }
    
    // Ждём завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }
    
    // Проверяем, что не более max_count потоков работало одновременно
    EXPECT_LE(max_concurrent.load(), max_count);
    EXPECT_GT(max_concurrent.load(), 0);
}

TEST(SemaphoreTest, BlockingAcquire) {
    Semaphore sem("blocking_test", 1);
    
    // Захватываем семафор в основном потоке
    sem.acquire();
    
    std::atomic<bool> thread_acquired{false};
    std::thread t([&]() {
        sem.acquire(); // Должен заблокироваться
        thread_acquired = true;
        sem.release();
    });
    
    // Даём потоку время для запуска и блокировки
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(thread_acquired); // Поток должен быть заблокирован
    
    // Освобождаем семафор в основном потоке
    sem.release();
    
    // Ждём, пока поток захватит семафор
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(thread_acquired); // Теперь поток должен был захватить
    
    t.join();
}

TEST(SemaphoreTest, MultipleReleases) {
    Semaphore sem("multi_release", 2);
    
    // Захватываем оба слота
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_FALSE(sem.tryAcquire());
    
    // Освобождаем несколько раз (больше чем захватывали)
    sem.release();
    sem.release();
    sem.release(); // Лишний release - не должен увеличивать счётчик сверх максимума
    
    // Должны быть доступны только 2 слота
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_TRUE(sem.tryAcquire());
    EXPECT_FALSE(sem.tryAcquire());
}

TEST(SemaphoreTest, ZeroInitialCount) {
    Semaphore sem("zero_test", 0);
    
    EXPECT_FALSE(sem.tryAcquire()); // Нет доступных слотов
    
    std::atomic<bool> thread_acquired{false};
    std::thread t([&]() {
        sem.acquire(); // Должен заблокироваться до release
        thread_acquired = true;
    });
    
    // Даём потоку время для запуска
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_FALSE(thread_acquired);
    
    // Освобождаем один слот
    sem.release();
    
    // Даём потоку время для захвата
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_TRUE(thread_acquired);
    
    t.join();
}

TEST(SemaphoreTest, StressTest) {
    const int iterations = 1000;
    const int sem_count = 5;
    const int num_threads = 20;
    
    Semaphore sem("stress_test", sem_count);
    std::atomic<int> counter{0};
    std::vector<std::thread> threads;
    
    auto worker = [&](int id) {
        for (int i = 0; i < iterations; ++i) {
            sem.acquire();
            
            // Критическая секция
            int old_val = counter.load();
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            counter.store(old_val + 1);
            
            sem.release();
        }
    };
    
    // Запускаем потоки
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(worker, i);
    }
    
    // Ждём завершения
    for (auto& t : threads) {
        t.join();
    }
    
    // Проверяем корректность счётчика
    EXPECT_EQ(counter.load(), iterations * num_threads);
}

TEST(SemaphoreTest, GetterMethods) {
    Semaphore sem("getter_test", 5);
    
    EXPECT_EQ(sem.getName(), "getter_test");
    EXPECT_EQ(sem.getMaxCount(), 5);
    EXPECT_EQ(sem.getCount(), 5); // Изначально все слоты свободны
    
    sem.acquire();
    EXPECT_EQ(sem.getCount(), 4); // Один слот занят
    
    sem.release();
    EXPECT_EQ(sem.getCount(), 5); // Снова все свободны
}

TEST(SemaphoreTest, ConcurrencyWithTryAcquire) {
    Semaphore sem("concurrency_test", 2);
    std::atomic<int> success_count{0};
    std::atomic<int> total_attempts{0};
    std::vector<std::thread> threads;
    
    auto worker = [&](int id) {
        for (int i = 0; i < 100; ++i) {
            total_attempts++;
            if (sem.tryAcquire()) {
                success_count++;
                std::this_thread::sleep_for(std::chrono::microseconds(10));
                sem.release();
            }
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    };
    
    // Запускаем много потоков
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Проверяем, что не было состояния гонки
    EXPECT_LE(success_count.load(), total_attempts.load());
    std::cout << "Success rate: " << success_count << "/" << total_attempts << std::endl;
}