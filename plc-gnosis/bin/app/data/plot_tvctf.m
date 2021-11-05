function [] = plot_tvctf(freqAxis, timeAxis, data)

figure;
imagesc(freqAxis, timeAxis, 20*log10(abs(data)));
colorbar;
xlabel('Frequency (Hz)');
ylabel('Time (s)');
