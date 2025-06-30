import sys
import os
cur_dir = os.path.dirname(__file__)
pyd_dir = os.path.join(cur_dir, 'build/Release')
sys.path.append(os.path.abspath(pyd_dir))

import SPO2Algorithm
import csv
import argparse
import matplotlib.pyplot as plt

def read_column_from_csv(file_path, column_index):
      values = []
      with open(file_path, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        for row in reader:
          if len(row) > column_index:
            try:
              values.append(float(row[column_index]))
            except ValueError:
              continue  # skip rows where conversion fails
      return values

def split_into_chunks(data, chunk_size):
      return [data[i:i+chunk_size] for i in range(0, len(data), chunk_size) if len(data[i:i+chunk_size]) == chunk_size]

def main():
  # define constants
  chunk_size = 200
  sampling_rate = 10

  # default calibration coefficients from Table 1 (https://www.analog.com/en/resources/technical-articles/guidelines-for-spo2-measurement--maxim-integrated.html)
  coeff_1 = 1.5958422 
  coeff_2 = -34.6596622
  coeff_3 = 112.6898759

  parser = argparse.ArgumentParser(description='Run SPO2 algorithm on PPG data.')
  parser.add_argument('data_folder', nargs='?', default=os.path.join(cur_dir, 'data'),
            help='Path to the folder containing PR.csv, PI.csv, and UN.csv files')
  args = parser.parse_args()
  data_folder = args.data_folder

  # load red (PR) and IR (PI) data from data folder
  pr_file = None
  for fname in os.listdir(data_folder):
    if fname.endswith('PR.csv'):
      pr_file = os.path.join(data_folder, fname)
      break
  if pr_file is None:
    raise FileNotFoundError("No CSV file ending with 'PR.csv' found in the data folder.")
  print(f"Found PR file: {pr_file}")
  pi_file = None
  for fname in os.listdir(data_folder):
    if fname.endswith('PI.csv'):
      pi_file = os.path.join(data_folder, fname)
      break
  if pi_file is None:
    raise FileNotFoundError("No CSV file ending with 'PI.csv' found in the data folder.")
  print(f"Found PI file: {pi_file}")

  # load UN data from data folder
  un_file = None
  for fname in os.listdir(data_folder):
    if fname.endswith('UN.csv'):
      un_file = os.path.join(data_folder, fname)
      break
  if un_file is None:
    raise FileNotFoundError("No CSV file ending with 'UN.csv' found in the data folder.")
  print(f"Found UN file: {un_file}")

  ppg_red = read_column_from_csv(pr_file, 7)
  ppg_red_ts = read_column_from_csv(pr_file, 1)
  ppg_ir = read_column_from_csv(pi_file, 7)
  ppg_ir_ts = read_column_from_csv(pi_file, 1)

  un = read_column_from_csv(un_file, 7)
  un_ts = read_column_from_csv(un_file, 1)

  # Split ppg_red and ppg_ir into chunks
  ppg_red_chunks = split_into_chunks(ppg_red, chunk_size)
  ppg_ir_chunks = split_into_chunks(ppg_ir, chunk_size)

  print(f"Number of ppg_red chunks: {len(ppg_red_chunks)}")
  print(f"Number of ppg_ir chunks: {len(ppg_ir_chunks)}")

  o2_levels = []
  o2_ts = []
  index = 0
  for red_chunk, ir_chunk in zip(ppg_red_chunks, ppg_ir_chunks):
    o2 = SPO2Algorithm.get_oxygen_level(ir_chunk, red_chunk, sampling_rate, coeff_1, coeff_2, coeff_3)
    o2_levels.append(o2)
    o2_ts.append(ppg_red_ts[index])
    index += chunk_size

  # Save o2_levels and o2_ts to a CSV file
  output_csv = os.path.join(data_folder, 'generated.csv')
  with open(output_csv, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(['Timestamp', 'O2_Level'])
    for ts, o2 in zip(o2_ts, o2_levels):
      writer.writerow([ts, o2])
  print(f"O2 levels and timestamps saved to {output_csv}")

  # Plot the full ppg_red, ppg_ir, and normalized un data for visualization with a secondary y-axis for UN
  fig, ax1 = plt.subplots(figsize=(12, 6))
  ax1.plot(ppg_red_ts, ppg_red, label='PPG Red (PR)')
  ax1.plot(ppg_ir_ts, ppg_ir, label='PPG IR (PI)')
  ax1.set_xlabel('EmotiBit Timestamp')
  ax1.set_ylabel('Amplitude (PPG)')
  ax1.legend(loc='upper left')

  ax2 = ax1.twinx()
  ax2.plot(un_ts, un, color='tab:green', label='SPO2 from 3rd Party')
  ax2.plot(o2_ts, o2_levels, color='tab:orange', label='SPO2 using Brainflow')
  ax2.set_ylabel('SPO2')
  ax2.legend(loc='upper right')

  plt.title('PPG Red, IR, and SPO2 Signals')
  plt.tight_layout()
  plt.show()

if __name__ == "__main__":
    main()
