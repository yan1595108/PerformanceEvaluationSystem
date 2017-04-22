load_system('QAM16_100K_I2Q2');
sim('QAM16_100K_I2Q2');
close_system('QAM16_100K_I2Q2');
fid = fopen('result.dat', 'wb');
fwrite(fid, result.signals(1).values, 'char');
fclose(fid);