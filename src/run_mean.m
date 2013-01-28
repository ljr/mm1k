1;

c=csvread ("results/out_40-38-consolidated.csv" );
csvwrite ("results/out-mean.csv",[c(:,1) mean(c(:,2:end),2)]);


