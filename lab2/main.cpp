#include <iostream>
#include <pthread.h>
#include <vector>
#include <algorithm>
#include <climits>
#include <cfloat>
#include <thread>
#include <chrono>
#include <iomanip>

struct MatrixData {
    std::vector<std::vector<float>> matrix;
    int k;
    int num_threads;
};

std::vector<std::vector<float>> erosion_matrix;
std::vector<std::vector<float>> dilation_matrix;

struct ThreadRange {
    int start_row;
    int end_row;
};

struct ThreadInfo {
    int thread_id;
    int rows_processed;
    std::chrono::microseconds execution_time;
};

std::vector<ThreadInfo> erosion_threads_info;
std::vector<ThreadInfo> dilation_threads_info;

void applyMorphOperation(const std::vector<std::vector<float>>& input,
                        std::vector<std::vector<float>>& output,
                        int start_row, int end_row, bool is_erosion,
                        ThreadInfo& thread_info) {
    auto start_time = std::chrono::high_resolution_clock::now();
    int cols = input[0].size();
    int rows_processed = 0;
    
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < cols; j++) {
            float extreme_val = is_erosion ? FLT_MAX : -FLT_MAX;
            bool found = false;
            
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int ni = i + di;
                    int nj = j + dj;
                    if (ni >= 0 && ni < input.size() && nj >= 0 && nj < cols) {
                        found = true;
                        if (is_erosion) {
                            if (input[ni][nj] < extreme_val) {
                                extreme_val = input[ni][nj];
                            }
                        } else {
                            if (input[ni][nj] > extreme_val) {
                                extreme_val = input[ni][nj];
                            }
                        }
                    }
                }
            }
            
            if (found) {
                output[i][j] = extreme_val;
            }
        }
        rows_processed++;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    thread_info.execution_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    thread_info.rows_processed = rows_processed;
}

void* erosionFilter(void* arg) {
    auto filter_start = std::chrono::high_resolution_clock::now();
    
    MatrixData* data = static_cast<MatrixData*>(arg);
    const auto& input_matrix = data->matrix;
    int k = data->k;
    int num_threads = data->num_threads;
    
    int rows = input_matrix.size();
    int cols = input_matrix[0].size();
    
    std::vector<std::vector<float>> current = input_matrix;
    std::vector<std::vector<float>> next = input_matrix;
    
    std::vector<ThreadRange> ranges(num_threads);
    int rows_per_thread = (rows + num_threads - 1) / num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        ranges[t].start_row = t * rows_per_thread;
        ranges[t].end_row = std::min(ranges[t].start_row + rows_per_thread, rows);
    }
    
    erosion_threads_info.resize(num_threads);
    
    for (int iter = 0; iter < k; iter++) {
        std::vector<std::thread> threads;
        
        for (int t = 0; t < num_threads; t++) {
            if (ranges[t].start_row < rows) {
                erosion_threads_info[t].thread_id = t;
                threads.emplace_back(applyMorphOperation, std::cref(current), std::ref(next),
                                   ranges[t].start_row, ranges[t].end_row, true,
                                   std::ref(erosion_threads_info[t]));
            }
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        std::swap(current, next);
    }
    
    auto filter_end = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(filter_end - filter_start);
    
    erosion_matrix = current;
    return NULL;
}

void* dilationFilter(void* arg) {
    auto filter_start = std::chrono::high_resolution_clock::now();
    
    MatrixData* data = static_cast<MatrixData*>(arg);
    const auto& input_matrix = data->matrix;
    int k = data->k;
    int num_threads = data->num_threads;
    
    int rows = input_matrix.size();
    int cols = input_matrix[0].size();
    
    std::vector<std::vector<float>> current = input_matrix;
    std::vector<std::vector<float>> next = input_matrix;
    
    std::vector<ThreadRange> ranges(num_threads);
    int rows_per_thread = (rows + num_threads - 1) / num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        ranges[t].start_row = t * rows_per_thread;
        ranges[t].end_row = std::min(ranges[t].start_row + rows_per_thread, rows);
    }
    
    dilation_threads_info.resize(num_threads);
    
    for (int iter = 0; iter < k; iter++) {
        std::vector<std::thread> threads;
        
        for (int t = 0; t < num_threads; t++) {
            if (ranges[t].start_row < rows) {
                dilation_threads_info[t].thread_id = t;
                threads.emplace_back(applyMorphOperation, std::cref(current), std::ref(next),
                                   ranges[t].start_row, ranges[t].end_row, false,
                                   std::ref(dilation_threads_info[t]));
            }
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        std::swap(current, next);
    }
    
    auto filter_end = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::microseconds>(filter_end - filter_start);
    dilation_matrix = current;
    return NULL;
}

void printMatrix(const std::vector<std::vector<float>>& mat) {
    for (const auto& row : mat) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    pthread_t erosion, dilation;

    int rows, cols, k, num_threads;
    float el;
    
    std::cout << "Type the amount of rows, columns, k and number of threads: ";
    std::cin >> rows >> cols >> k >> num_threads;

    std::vector<std::vector<float>> original_matrix(rows, std::vector<float>(cols));
    erosion_matrix.resize(rows, std::vector<float>(cols));
    dilation_matrix.resize(rows, std::vector<float>(cols));

    std::cout << "Fill in the " << rows << " by " << cols << " matrix:\n";

    for (int i = 0; i != rows; ++i) {
        for (int j = 0; j != cols; ++j) {
            std::cin >> el;
            original_matrix[i][j] = el;
        }
    }

    std::cout << "\nInitial matrix:\n";
    printMatrix(original_matrix);

    std::cout << "\n=== PROGRAM CONFIGURATION ===" << std::endl;
    std::cout << "Matrix size: " << rows << " x " << cols << std::endl;
    std::cout << "Number of iterations (k): " << k << std::endl;
    std::cout << "Number of threads: " << num_threads << std::endl;
    std::cout << "Total elements: " << rows * cols << std::endl;

    auto program_start = std::chrono::high_resolution_clock::now();
    
    MatrixData data{original_matrix, k, num_threads};

    if (pthread_create(&erosion, NULL, erosionFilter, &data) != 0) {
        std::cerr << "pthread_create erosion failed\n";
        return 1;
    }
    if (pthread_create(&dilation, NULL, dilationFilter, &data) != 0) {
        std::cerr << "pthread_create dilation failed\n";
        return 1;
    }

    pthread_join(erosion, NULL);
    pthread_join(dilation, NULL);

    auto program_end = std::chrono::high_resolution_clock::now();
    auto total_program_time = std::chrono::duration_cast<std::chrono::microseconds>(program_end - program_start);

    std::cout << "\n=== FINAL RESULTS ===" << std::endl;
    std::cout << "Total program execution time: " << total_program_time.count() << " microseconds" << std::endl;
    
    std::cout << "\nErosion matrix after " << k << " iterations:\n";
    printMatrix(erosion_matrix);

    std::cout << "\nDilation matrix after " << k << " iterations:\n";
    printMatrix(dilation_matrix);

    return 0;
}