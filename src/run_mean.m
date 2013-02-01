1;

c=csvread ("results/out_consolidated.csv" );

csize=size(c(:,2:end))(2);
cmean=mean(c(:,2:end),2);
sdv=std(c(:,2:end),0,2);
confhi=cmean.+(1.960).*sdv./sqrt(csize);
conflo=cmean.-(1.960).*sdv./sqrt(csize);

csvwrite ("results/out-mean.csv",[c(:,1) cmean(:,1) conflo(:,1) confhi(:,1)]);


