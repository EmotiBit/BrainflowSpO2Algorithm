# Description
This algorithm is derived from the Brainflow SpO2 algorithm, and uses PPG-R and PPG-IR data from the EmotiBit to calculate SPO2 level. As a starting point, `run.py` splits PPG-R and PPG-IR signals into buffers of length 64, which are then passed to the algorithm. It is recommended for the buffers to contain **at least** 64 samples for accurate results.

# Required Hardware
- Emotibit

# Scripts
## `run.py <path_to_data_folder>`
Runs the Brainflow algorithm on the provided data folder (which needs to contain a separate .csv file for PPG-R and PPG-IR from the EmotiBit ending in `PR.csv` and `PI.csv`) and saves the calculated SpO2 with timestamps to `generated.csv`. Also shows a plot of the PPG data overlayed with the calculated SpO2.

# Performance
## [sit-stand-sit_v0.0.0](https://github.com/EmotiBit/Biometric_Validation_Methods/releases/tag/sit-stand-sit_v0.0.0)
Device | Resample Test | Scatterplot | Mean-Difference Plot
--- | --- | --- | ---
IP900AP | ![IP900AP Resample](assets/tests/sit-stand-sit_v0.0.0/ip900ap/ip900ap_resampled.png) | ![IP900AP Scatterplot](assets/tests/sit-stand-sit_v0.0.0/ip900ap//ip900ap_scatter.png) | ![IP900AP Mean Difference Plot](assets/tests/sit-stand-sit_v0.0.0/ip900ap//ip900ap_mean-diff.png)
## [simulated-unobstructed-airway_v0.0.0](https://github.com/EmotiBit/Biometric_Validation_Methods/releases/tag/simulated-unobstructed-airway_v0.0.0)
Device | Resample Test | Scatterplot | Mean-Difference Plot
--- | --- | --- | ---
IP900AP | ![IP900AP Resample](assets/tests/simulated-unobstructed-airway_v0.0.0/ip900ap/ip900ap_resampled.png) | ![IP900AP Scatterplot](assets/tests/simulated-unobstructed-airway_v0.0.0/ip900ap//ip900ap_scatter.png) | ![IP900AP Mean Difference Plot](assets/tests/simulated-unobstructed-airway_v0.0.0/ip900ap//ip900ap_mean-diff.png)
