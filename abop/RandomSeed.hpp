#pragma on
#include <random>
#include <chrono>

// random - 난수 생성 : dist(gen)
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::mt19937 gen(seed);
std::uniform_int_distribution<std::mt19937::result_type> dist;