#ifndef SPO2_ALGORITHM_H
#define SPO2_ALGORITHM_H

#include <math.h>
#include <stdlib.h>
#include "DspFilters/Dsp.h"

// These constants are from Table 1 (https://www.analog.com/en/resources/technical-articles/guidelines-for-spo2-measurement--maxim-integrated.html)
#define CALIB_COEFF_1 1.5958422
#define CALIB_COEFF_2 -34.6596622
#define CALIB_COEFF_3 112.6898759

#define FILTER_SAMPLING_RATE 10 // Hz

enum class FilterTypes : int
{
    BUTTERWORTH = 0,
    CHEBYSHEV_TYPE_1 = 1,
    BESSEL = 2,
    BUTTERWORTH_ZERO_PHASE = 3,
    CHEBYSHEV_TYPE_1_ZERO_PHASE = 4,
    BESSEL_ZERO_PHASE = 5
};

enum class DetrendOperations : int
{
    NO_DETREND = 0,
    CONSTANT = 1,
    LINEAR = 2
};

inline float rms (float x[], int n)
{
    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += x[i] * x[i];
    }
    return sqrt (sum / n);
}

inline float mean (float x[], int n)
{
    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += x[i];
    }
    return sum / n;
}

inline void reverse_array (float data[], int len)
{
    for (int i = 0; i < len / 2; i++)
    {
        float temp = data[i];
        data[i] = data[len - i - 1];
        data[len - i - 1] = temp;
    }
}

void get_oxygen_level (float *ppg_ir, float *ppg_red, int data_size, float *oxygen_level);
void detrend (float *data, int data_len, int detrend_operation);
void perform_bandpass (float *data, int data_len, int sampling_rate, float start_freq, float stop_freq, int order, int filter_type, float ripple);

#endif /* SPO2_ALGORITHM_H */