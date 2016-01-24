close all
clear all

data = load('timeLog.txt');

x = (data(:,4)-data(1,4))/3600;
yS = data(:,3)-data(:,2);
yN = data(:,4)-data(:,2);

p=polyfit(x,yN,1);
yP = polyval(p,x);
ppm = abs(p(1)/3600)*1000000;
ppmstr = sprintf('%0.02f ppm',ppm);

figure()
plot(x,yS,'-g',
     x,yN,'-r',
     x,yP,'-.b')
xlabel('t [h]')
ylabel('dt [s]')
legend(['system';'network';ppmstr])
title('Time base stability test (compare with system and network time)')
grid on

print('dpng','timeTest.png')