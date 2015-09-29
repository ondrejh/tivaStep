close all
clear all

data = load('timeLog.txt');
figure()
plot((data(:,4)-data(1,4))/3600,data(:,3)-data(:,2),'-g',
     (data(:,4)-data(1,4))/3600,data(:,4)-data(:,2),'-r')
xlabel('t [h]')
ylabel('dt [s]')
legend(['system';'network'])
title('Time base stability test (compare with system and network time)')
grid on

print('dpng','timeTest.png')