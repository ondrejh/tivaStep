close all
clear all

dt = 0.01; %s
Amax = 5000; %Hz/s
Vmax = 5000; %Hz

p = 10;
s = 0;
v = 0;
a = 0;

ke = 1000;
kv = 25;

cnt = 0;
data = [cnt*dt,a,v,s];

function ret = clamp(x,Xmax)
	if (x>Xmax)
		ret = Xmax;
	elseif  (x<-Xmax)
		ret = -Xmax;
	else
		ret = x;
	endif
endfunction

do
	a = (p-s)*ke;
	a -= v*kv;
	a = clamp(a,Amax);
	v = v + a*dt;
	v = clamp(v,Vmax);
	s = s + v*dt;
	cnt += 1;
	data = [data;cnt*dt,a,v,s];
	if (round(s)==round(p))
		if (abs(v)<1)
			if (abs(a)<1)
				break
				done = 1;
			endif
		endif
	endif
until (cnt>10000);

figure()
subplot(3,1,1)
plot(data(:,1),data(:,4))
grid on
subplot(3,1,2)
plot(data(:,1),data(:,3))
grid on
subplot(3,1,3)
plot(data(:,1),data(:,2))
grid on
