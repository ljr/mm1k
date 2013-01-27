out <- read.table("/data/usp/doutorado/quali/experimentos/simpack/full.csv", sep=";", quote="\"")
out$avg = apply(out[,2:201],1,mean,na.rm=TRUE)
plot(out$avg,type="l")

