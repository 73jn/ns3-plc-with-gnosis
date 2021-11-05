function [] = plot_ctf(fileName, colour)

if nargin < 2
	colour = 'k';
end

[freqAxis timeAxis data] = load_ctf(fileName);

if(numel(freqAxis) == 1)
	plot_fivctf(timeAxis, data, colour);
	return;
end

if(numel(timeAxis) == 1)
	plot_tivctf(freqAxis, data, colour);
	return;
end

plot_tvctf(freqAxis, timeAxis, data);
return;
