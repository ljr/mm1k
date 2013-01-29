#efeito da entrada na saída.
u=0.1:.1:10;
v2=[]; for i = 1:100; v2 = [v2 2]; endfor
u2=v2./u;
v3=[]; for i = 1:100; v3 = [v3 3.]; endfor
u3=v3./u;
v35=[]; for i = 1:100; v35 = [v35 3.5]; endfor
u35=v3.5/u;
v38=[]; for i = 1:100; v38 = [v38 3.8]; endfor
u38=v38./u;
v50=[]; for i = 1:100; v50 = [v50 5.0]; endfor
u50=v50./u;
plot(u,u2,u,u3,u,u35,u,u38,u,u50)



out=dlmread ("identification.csv" ,";")
run("/usr/share/octave/packages/control-2.4.1/@lti/dcgain.m")


t=csvread("oct")

yp=t(:,2);
size(yp)

up=t(:,3);
size(up)

mu = mean(up(1:end-1))
my = mean(yp(2:end))
u = up - mu;
size(u)
y = yp - my;
size(y)

H = [y(1:end-1) u(1:end-1)];
size(H)
size(y(2:end))

T = H\y(2:end)


yhat = T(1)*y(1:end-1) + T(2)*u(1:end-1);
plotyy(out(:,1),out(:,2),out(:,1),out(:,4))
plot(y(2:end),yhat, ' * ',y,y,' -')


rmse=sqrt(sum(power(y(2:end) - yhat, 2))/size(yhat)(1))
r2=1-(var(y(2:end)-yhat)/var(y(2:end)))

mm1=tf(T(2),[1 T(1)], 300)
impulse(mm1,2000)
step(mm1,2000)

