myfiles <- list.files(path = "/home/christoph/repos/ns-3-allinone/ns-3-dev/lenaresults/onoff", full.names = TRUE, recursive = FALSE)
print(myfiles)
for (file in myfiles) {
	#setwd(dir)
	#print(getwd())
	data <- read.table(file, sep=" ", dec=".", header=F)
	
	png(paste(file, "_bytes", ".png", sep=""), height = 1080, width = 1920)
	plot(data[,1], data[,2], type="o", main=paste("Bytes"), xlab="Time (seconds)", ylab="Data (bytes)")
	dev.off()

	png(paste(file, "_totalBytes", ".png", sep=""), height = 1080, width = 1920)
	plot(data[,1], data[,3], type="o", main=paste("Total Bytes"), xlab="Time (seconds)", ylab="Data (bytes)")
	dev.off()

	png(paste(file, "_bw", ".png", sep=""), height = 1080, width = 1920)
	plot(data[,1], ((data[,3]*8*1e-6)/(data[,1]-1)), type="o", main=paste("Bandwidth"), xlab="Time (seconds)", ylab="BW (mbit/s)")
	dev.off()
}