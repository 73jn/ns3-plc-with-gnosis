function [] = plot_tivctf(freqAxis, data, color)

if(rows(data) ~= 1)
	data = data';
end

plot(freqAxis,20*log10(abs(data)), color);
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
