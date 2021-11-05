function [] = write_vdf(filename, dataMatrix)

csvwrite(filename, [real(dataMatrix);imag(dataMatrix)]);
