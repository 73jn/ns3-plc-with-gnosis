function [freqAxis timeAxis data] = load_ctf(fileName)

freqData = dlmread(fileName, ',', [0, 0, 0, 2]);
timeData = dlmread(fileName, ',', [1, 0, 1, 1]);
ctfData = dlmread(fileName, ',', 2, 0);

freqAxis = linspace(freqData(1), freqData(2), freqData(3));
timeAxis = linspace(0, timeData(1), timeData(2));
dataSize = size(ctfData);
data = ctfData(1 : dataSize(1)/2, :) + i*ctfData(dataSize(1)/2 + 1 : end, :);

