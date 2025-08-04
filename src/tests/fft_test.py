import numpy as np

freq_amp = [(8.0, 8.0), (131.5, 5.0), (357.7, 3.0)]
test_signal_length = 1024
test_signal = np.zeros(test_signal_length, dtype=np.float32)
for i in range(test_signal_length):
    sample = 0
    for fa in freq_amp:
        freq = fa[0]
        amp = fa[1]
        sample += amp * np.sin(2 * np.pi * freq * i / float(test_signal_length))
    test_signal[i] = sample

test_result = np.fft.fft(test_signal).astype(np.complex64)

with open("../../data/test/fft_test_signal.float", 'wb') as f:
    test_signal.tofile(f)

with open("../../data/test/fft_test_result.float", 'wb') as f:
    test_result.tofile(f)
