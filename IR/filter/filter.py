import numpy as np
from scipy.signal import butter, filtfilt

if __name__ == "__main__":
    with open("./rawData.txt") as f:
        lines = f.readlines()
        data = []
        for i in lines:
            data.append(int(i.replace("\n","")))

    # exponential weighted moving average
    weight = 0.2
    previousReading = 0.0
    emwfData = []
    for i in data:
        emwf = (1 - weight) * previousReading + weight * int(i)
        emwfData.append(emwf)
        previousReading = emwf

    # butterworth low pass filter 
    cutoff = 100
    nyq = 0.5 * len(data)
    normal = cutoff / nyq

    b,a = butter(2, normal, btype="low", analog=False)
    butterData = filtfilt(b,a,data)

    with open("./butterData.txt",'w') as f:
        for i in butterData:
            f.write(f"{i}\n")