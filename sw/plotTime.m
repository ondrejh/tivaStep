close all
clear all

data = load('timeLog.txt')
figure()
plot((data(:,1)-data(1,1))/3600,data(:,2))
xlabel('t [h]')
ylabel('dt [s]')
grid on