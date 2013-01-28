1;

c=csvread ("results/out_consolidated.csv" );
csvwrite ("results/out-mean.csv",[c(:,1) mean(c(:,2:end),2)]);


