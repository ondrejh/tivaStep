close all
clear all

bodA = 8000; % kroku
bodB = 5000; % kroku

dt = 0.0001; % casovy krok simulace

constA = 5000; % kroku za sekundu^2
constV = 2000; % kroku za sekundu
constVmin = 100;

v0 = 0;%constV; % vychozi rychlost

t = 0;
bodP = bodA;
a = 0;
v = v0;

data = [t,bodP,v];

for i=(0:100000)
  smerA = 0;
  if ((round(bodB-bodP))>0)
    smerA = constA;
  elseif ((round(bodB-bodP))<0)
    smerA = -constA;
  else
    smerA = 0;
    if abs(v)<constVmin
      v=0;
    endif
  endif
  t = t+dt;
  a = smerA;
  if (sign(v)!=sign(smerA))
    a=smerA;
  else
    tZast = v/smerA;
    sZast = abs(v*tZast/2);
    if abs(bodB-bodP)>sZast
      a=smerA;
    else
      a=-smerA;
    endif
  endif
  v = v+a*dt;
  if (abs(v)>constV)
    v=sign(v)*constV;
  endif
  bodP = bodP+v*dt;
  data = [data;[t,bodP,v]];
  if v==0
    if round(bodB)==round(bodP)
      break
    endif
  endif
endfor

figure()
subplot(3,1,1:2)
plot(data(:,1),data(:,2))
ylabel('s [steps]')
grid on
subplot(3,1,3)
plot(data(:,1),data(:,3))
ylabel('v [Hz]')
xlabel('t [s]')
grid on
