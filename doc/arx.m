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


t=csvread("02-results/r-29_32.csv");
t=csvread("02-results/r-29_35.csv");
t=csvread("02-results/r-29_41.csv");
t=csvread("02-results/r-35_40.csv");

yp=t(:,2);
up=t(:,5);

mu = mean(up(1:end-1))
my = mean(yp(2:end))
u = up - mu;
y = yp - my;

H = [y(1:end-1) u(1:end-1)];
T = H\y(2:end)

yhat = T(1)*y(1:end-1) + T(2)*u(1:end-1);
rmse=sqrt(sum(power(y(2:end) - yhat, 2))/size(yhat)(1))
r2=1-(var(y(2:end)-yhat)/var(y(2:end)))

plot(y(2:end),yhat, ' * ',y,y,' -')
xlabel("Valores medidos")
ylabel("Valores preditos")
legend("Valores experimentais", "Valores do modelo")




print -color -deps 04-coef_29_32.eps
mu =  30.632 my =  371.94
a 0.96487 b -4.34423
rmse =  7.3648 r2 =  0.99617

print -color -deps 04-coef_29_35.eps
mu =  34.440 my =  233.11
a 0.97163 b -1.14608
rmse =  6.8379 r2 =  0.99740 

print -color -deps 04-coef_29_41.eps
mu =  35.528 my =  219.43
a 0.97627 b -0.80957
rmse =  7.2492 r2 =  0.99712

print -color -deps 04-coef_35_40.eps



plotyy(out(:,1),out(:,2),out(:,1),out(:,4))


#rmse=sqrt(sum(power(y(2:end) - yhat, 2))/size(yhat)(1))
#r2=1-(var(y(2:end)-yhat)/var(y(2:end)))

mm1=tf(T(2),[1 T(1)], 300)
step(mm1,2000)
impulse(mm1,2000)


ssb=repmat(28/(1-28/34.44),1,size(t34(:,1))(1)/3);
ssa=repmat(28/(1-28/(34.44-4.718)),1,2*size(t34(:,1))(1)/3);
ss = [0 ssb ssa];

predmyb=repmat(233.11,1,size(t34(:,1))(1)/3);
uss=34.44-4.718;a=0.97163; b=-1.14608;
predmya=repmat((b/(1-a))*uss,1,2*size(t34(:,1))(1)/3);
yss=[0 predmyb predmya];


