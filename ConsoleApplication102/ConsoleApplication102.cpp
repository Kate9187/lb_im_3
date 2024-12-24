#include <iostream>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <random>
#include <array>
#include <vector>

const int number_count = 100;
int numBins = 10; //количество интервалов для теста

class MersenneTwister {
public:
    static const int N = 624;           // Размер массива состояния
    static const int M = 397;           // Параметр алгоритма
    static const unsigned int MATRIX_A = 0x9908b0df; // Константа для смешивания
    static const unsigned int UPPER_MASK = 0x80000000; // Маска для старшего бита
    static const unsigned int LOWER_MASK = 0x7fffffff; // Маска для младших битов

    MersenneTwister(unsigned int seed) {
        // Инициализация массива состояния
        mt[0] = seed;
        for (mti = 1; mti < N; ++mti) {
            mt[mti] = (1812433253 * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
            mt[mti] &= 0xffffffff; // Для обеспечения 32-битности
        }
    }

    unsigned int extractNumber() {
        if (mti >= N) {
            twist(); // Если массив состояния исчерпан, обновляем его
        }

        unsigned int y = mt[mti++];
        // Применяем "темперирование" для улучшения случайности
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680;
        y ^= (y << 15) & 0xefc60000;
        y ^= (y >> 18);

        return y;
    }

private:
    std::array<unsigned int, N> mt; // Массив состояния
    int mti = N + 1; // Индекс для массива состояния

    void twist() {
        for (int i = 0; i < N; ++i) {
            unsigned int y = (mt[i] & UPPER_MASK) | (mt[(i + 1) % N] & LOWER_MASK);
            mt[i] = mt[(i + M) % N] ^ (y >> 1);
            if (y % 2 != 0) {
                mt[i] ^= MATRIX_A; // Используем константу для смешивания
            }
        }
        mti = 0; // Сбрасываем индекс
    }
};


float* mersenneTwister(int n, int seed) {

    float* r = new float[n];
    if (seed == -1) {
        std::random_device rd;
        MersenneTwister mt(rd());
        for (int i = 0; i < n; i++) {
            r[i] = mt.extractNumber() * (1.0 / 4294967296.0); //диапазон [0, 1) для чисел с плавающей точкой
        }
    }
    else {
        MersenneTwister mt(seed);
        for (int i = 0; i < n; i++) {
            r[i] = mt.extractNumber() * (1.0 / 4294967296.0); 
        }
    }
    return r;
}

float* mersenneTwister_mt(int n, int seed) {

    float* r = new float[n];
    //генерация чисел с равномерным распределением в заданном диапазоне
    std::uniform_real_distribution<float> distr(0.0, 1.0);
    if (seed == -1) {
        std::random_device rd;
        //реализует алгоритм Mersenne Twister
        std::mt19937 gen(rd());
        for (int i = 0; i < n; i++) {
            r[i] = distr(gen);
        }
    }
    else {
        std::mt19937 gen(seed);
        for (int i = 0; i < n; i++) {
            r[i] = distr(gen);
        }
    }

    return r;
}

// Функция для вычисления хи-квадрат
double chiSquareTest(const float* data, int dataSize, int numBins) {
    //для хранения количества значений, попадающих в каждый интервал
    int* frequency = new int[numBins]();

    // Определяем частоты элементов
    for (int i = 0; i < dataSize; ++i) {
        float value = data[i];
        if (value >= 0.0f && value < 1.0f) {
            int binIndex = static_cast<int>(value * numBins);
            frequency[binIndex]++;
        }
    }
    //ожидаемая частота значений в каждом интервале для равномерного распределения
    double expectedFrequency = static_cast<double>(dataSize) / numBins;
    //для накопления суммы отклонений между наблюдаемыми и ожидаемыми частотами по всем интервалам
    double chiSquare = 0.0;

    // Вычисляем значение хи-квадрат
    for (int i = 0; i < numBins; ++i) {
        chiSquare += std::pow(static_cast<double>(frequency[i]) - expectedFrequency, 2) / expectedFrequency;
    }
    delete[] frequency;
    return chiSquare;
}

int main()
{
    float* mersenne_array;
    mersenne_array = mersenneTwister(number_count, -1);
    printf("mersenne_array\n");
    for (int i = 0; i < number_count; i++) {
        if ((i % 10 == 0) && (i != 0)) printf("\n");
        printf("%1.6f ", mersenne_array[i]);
    }
    printf("\n\n");

    float* mersenneMt_array;
    mersenneMt_array = mersenneTwister_mt(number_count, -1);
    printf("mersenneMt_array\n");
    for (int i = 0; i < number_count; i++) {
        if ((i % 10 == 0) && (i != 0)) printf("\n");
        printf("%1.6f ", mersenneMt_array[i]);
    }
    printf("\n\n");

    double chiSquare1 = chiSquareTest(mersenne_array, number_count, numBins);
    double chiSquare2 = chiSquareTest(mersenneMt_array, number_count, numBins);

    printf("Chi-square for mersenne_array: %1.2f\n", chiSquare1);
    printf("Chi-square for mersenneMt_array: %1.2f", chiSquare2);

    delete(mersenne_array);
    delete(mersenneMt_array);
}
