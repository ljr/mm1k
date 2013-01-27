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
# escolher v38
#    0~0.2 linear
#    0~0.4 almost linear
#    0~1.5 nonlinear



out=dlmread ("identification.csv" ,";")
run("/usr/share/octave/packages/control-2.4.1/@lti/dcgain.m")


out=csvread("oct")

yp=out(:,4);
size(yp)

up=out(:,2);
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

t = H\y(2:end)
# t =
#
#a->   9.9141e-01
#b->  -7.4134e-05


yhat = t(1)*y(1:end-1) + t(2)*u(1:end-1);
plotyy(out(:,1),out(:,2),out(:,1),out(:,4))
plot(y(2:end),yhat, ' * ',y,y,' -')


rmse=sqrt(sum(power(y(2:end) - yhat, 2))/size(yhat)(1));
r2=1-(var(y(2:end)-yhat)/var(y(2:end)))

mm1=tf(t(2),[1 t(1)], 40)
impulse(mm1,2000)
step(mm1,2000)

