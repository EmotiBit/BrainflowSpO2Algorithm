#include "spo2_algorithm.h"

#define MAX_FILTER_ORDER 8

/**
 * @brief Calculates the oxygen saturation level (SpO2) from PPG IR and RED signals.
 *
 * This function processes infrared (IR) and red photoplethysmogram (PPG) signals to estimate
 * the blood oxygen saturation level using filtering, detrending, and a calibration formula.
 *
 * @param ppg_ir         Pointer to the array of IR PPG signal samples.
 * @param ppg_red        Pointer to the array of RED PPG signal samples.
 * @param data_size      Number of samples in the PPG signal arrays.
 * @param oxygen_level   Pointer to a float where the calculated SpO2 value will be stored (0-100).
 */
void get_oxygen_level(float *ppg_ir, float *ppg_red, int data_size, float *oxygen_level)
{
    float *red_raw = new float[data_size];
    float *ir_raw = new float[data_size];
    int filter_shift = 25; // to get rif of filtereing artifact, dont use first elements
    int new_size = data_size - filter_shift;
    float *new_red_raw = red_raw + filter_shift;
    float *new_ir_raw = ir_raw + filter_shift;
    memcpy (red_raw, ppg_red, data_size * sizeof (float));
    memcpy (ir_raw, ppg_ir, data_size * sizeof (float));

    // need prefiltered mean of red and ir for dc
    float mean_red = mean (new_red_raw, new_size);
    float mean_ir = mean (new_ir_raw, new_size);

    // filtering(full size)
    detrend (red_raw, data_size, (int)DetrendOperations::CONSTANT);
    detrend (ir_raw, data_size, (int)DetrendOperations::CONSTANT);
    perform_bandpass (red_raw, data_size, FILTER_SAMPLING_RATE, 0.7, 1.5, 4, (int)FilterTypes::BUTTERWORTH, 0.0);
    perform_bandpass (ir_raw, data_size, FILTER_SAMPLING_RATE, 0.7, 1.5, 4, (int)FilterTypes::BUTTERWORTH, 0.0);

    // calculate AC & DC components using mean & rms:
    float redac = rms (new_red_raw, new_size);
    float irac = rms (new_ir_raw, new_size);
    float reddc = mean_red;
    float irdc = mean_ir;

    // https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6845.html
    float r = (redac / reddc) / (irac / irdc);
    float spo2 = (CALIB_COEFF_1 * r * r) + (CALIB_COEFF_2 * r) + (CALIB_COEFF_3);
    if (spo2 > 100.0)
    {
        spo2 = 100.0;
    }
    if (spo2 < 0)
    {
        spo2 = 0.0;
    }
    *oxygen_level = spo2;

    delete[] red_raw;
    delete[] ir_raw;
}

void detrend (float *data, int data_len, int detrend_operation)
{
    if (detrend_operation == (int)DetrendOperations::CONSTANT)
    {
        float mean = 0.0;
        // subtract mean from data
        for (int i = 0; i < data_len; i++)
        {
            mean += data[i];
        }
        mean /= data_len;
        for (int i = 0; i < data_len; i++)
        {
            data[i] -= mean;
        }
    }
    if (detrend_operation == (int)DetrendOperations::LINEAR)
    {
        // use mean and gradient to find a line
        float mean_x = (data_len - 1) / 2.0;
        float mean_y = 0;
        for (int i = 0; i < data_len; i++)
        {
            mean_y += data[i];
        }
        mean_y /= data_len;
        float temp_xy = 0.0;
        float temp_xx = 0.0;
        for (int i = 0; i < data_len; i++)
        {
            temp_xy += i * data[i];
            temp_xx += i * i;
        }
        float s_xy = temp_xy / data_len - mean_x * mean_y;
        float s_xx = temp_xx / data_len - mean_x * mean_x;
        float grad = s_xy / s_xx;
        float y_int = mean_y - grad * mean_x;
        for (int i = 0; i < data_len; i++)
        {
            data[i] = data[i] - (grad * i + y_int);
        }
    }
}

void perform_bandpass (float *data, int data_len, int sampling_rate, float start_freq, float stop_freq, int order, int filter_type, float ripple)
{
    float center_freq = (start_freq + stop_freq) / 2.0;
    float band_width = stop_freq - start_freq;
    Dsp::Filter *f = NULL;
    float *filter_data[1];
    filter_data[0] = data;

    switch (static_cast<FilterTypes> (filter_type))
    {
        case FilterTypes::BUTTERWORTH:
            f = new Dsp::FilterDesign<Dsp::Butterworth::Design::BandPass<MAX_FILTER_ORDER>, 1> ();
            break;
        case FilterTypes::BUTTERWORTH_ZERO_PHASE:
            f = new Dsp::FilterDesign<Dsp::Butterworth::Design::BandPass<MAX_FILTER_ORDER>, 1> ();
            break;
        case FilterTypes::CHEBYSHEV_TYPE_1:
            f = new Dsp::FilterDesign<Dsp::ChebyshevI::Design::BandPass<MAX_FILTER_ORDER>, 1> ();
            break;
        case FilterTypes::CHEBYSHEV_TYPE_1_ZERO_PHASE:
            f = new Dsp::FilterDesign<Dsp::ChebyshevI::Design::BandPass<MAX_FILTER_ORDER>, 1> ();
            break;
        case FilterTypes::BESSEL:
            f = new Dsp::FilterDesign<Dsp::Bessel::Design::BandPass<MAX_FILTER_ORDER>, 1> ();
            break;
        case FilterTypes::BESSEL_ZERO_PHASE:
            f = new Dsp::FilterDesign<Dsp::Bessel::Design::BandPass<MAX_FILTER_ORDER>, 1> ();
            break;
    }

    Dsp::Params params;
    params[0] = sampling_rate; // sample rate
    params[1] = order;         // order
    params[2] = center_freq;   // center freq
    params[3] = band_width;
    if ((filter_type == (int)FilterTypes::CHEBYSHEV_TYPE_1) ||
        (filter_type == (int)FilterTypes::CHEBYSHEV_TYPE_1_ZERO_PHASE))
    {
        params[3] = ripple; // ripple
    }
    f->setParams (params);
    f->process (data_len, filter_data);
    if ((filter_type == (int)FilterTypes::BUTTERWORTH_ZERO_PHASE) ||
        (filter_type == (int)FilterTypes::CHEBYSHEV_TYPE_1_ZERO_PHASE) ||
        (filter_type == (int)FilterTypes::BESSEL_ZERO_PHASE))
    {
        reverse_array (data, data_len);
        f->process (data_len, filter_data);
        reverse_array (data, data_len);
    }
    delete f;
}