import numpy as np
import matplotlib.pyplot as plt

N = 1024
freq = 440
SR = 48000

k = 1.0/float(SR)

t = np.arange(N)
sig = np.sin(2 * np.pi * float(freq) * k * t)
spec = np.fft.fft(sig)[:N//2]/N

f = np.fft.fftfreq(N, k)[:N//2]
plt.semilogx(f, 10*np.log10(spec.real ** 2 + spec.imag ** 2))
plt.title("spectrum")
plt.xlabel("freq (Hz)")
plt.xlim([2, 20000])
plt.ylabel("Power")
plt.grid(True, which="both", ls='-')
plt.show()

