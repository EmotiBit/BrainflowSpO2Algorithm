#include "spo2_algorithm.h"

#define MAX_FILTER_ORDER 8

void get_oxygen_level(double *ppg_ir, double *ppg_red, int data_size, int sampling_rate, double callib_coef1, double callib_coef2, double callib_coef3, double *oxygen_level)
{
    double *red_raw = new double[data_size];
    double *ir_raw = new double[data_size];
    int filter_shift = 25; // to get rif of filtereing artifact, dont use first elements
    int new_size = data_size - filter_shift;
    double *new_red_raw = red_raw + filter_shift;
    double *new_ir_raw = ir_raw + filter_shift;
    memcpy (red_raw, ppg_red, data_size * sizeof (double));
    memcpy (ir_raw, ppg_ir, data_size * sizeof (double));

    // need prefiltered mean of red and ir for dc
    double mean_red = mean (new_red_raw, new_size);
    double mean_ir = mean (new_ir_raw, new_size);

    // filtering(full size)
    detrend (red_raw, data_size, (int)DetrendOperations::CONSTANT);
    detrend (ir_raw, data_size, (int)DetrendOperations::CONSTANT);
    perform_bandpass (red_raw, data_size, sampling_rate, 0.7, 1.5, 4, (int)FilterTypes::BUTTERWORTH, 0.0);
    perform_bandpass (ir_raw, data_size, sampling_rate, 0.7, 1.5, 4, (int)FilterTypes::BUTTERWORTH, 0.0);

    // calculate AC & DC components using mean & rms:
    double redac = rms (new_red_raw, new_size);
    double irac = rms (new_ir_raw, new_size);
    double reddc = mean_red;
    double irdc = mean_ir;

    // https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6845.html
    double r = (redac / reddc) / (irac / irdc);
    double spo2 = callib_coef1 * r * r + callib_coef2 * r + callib_coef3;
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

void detrend (double *data, int data_len, int detrend_operation)
{
    if (detrend_operation == (int)DetrendOperations::CONSTANT)
    {
        double mean = 0.0;
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
        double mean_x = (data_len - 1) / 2.0;
        double mean_y = 0;
        for (int i = 0; i < data_len; i++)
        {
            mean_y += data[i];
        }
        mean_y /= data_len;
        double temp_xy = 0.0;
        double temp_xx = 0.0;
        for (int i = 0; i < data_len; i++)
        {
            temp_xy += i * data[i];
            temp_xx += i * i;
        }
        double s_xy = temp_xy / data_len - mean_x * mean_y;
        double s_xx = temp_xx / data_len - mean_x * mean_x;
        double grad = s_xy / s_xx;
        double y_int = mean_y - grad * mean_x;
        for (int i = 0; i < data_len; i++)
        {
            data[i] = data[i] - (grad * i + y_int);
        }
    }
}

void perform_bandpass (double *data, int data_len, int sampling_rate, double start_freq, double stop_freq, int order, int filter_type, double ripple)
{
    double center_freq = (start_freq + stop_freq) / 2.0;
    double band_width = stop_freq - start_freq;
    Dsp::Filter *f = NULL;
    double *filter_data[1];
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