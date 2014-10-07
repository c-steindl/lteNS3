args <- commandArgs(TRUE)
wd <- getwd()

for (directory in args) {
	setwd(directory)
	print(getwd())
	files <- c("buffer.log", "linkconfig.txt", "framelist.txt", "controls.log", "handover.log")
	data <- read.table(files[1], sep=" ", dec=".", header=F)
	contr <- read.table(files[4], sep=" ", dec=".", header=F)
	quality <- contr[contr[,2] == "qualityChanged",]
	y_quality <- rep(0, dim(quality)[1])
  ho_start <- c()
  ho_stop <- c()
	if (file.exists(files[5]) && file.info(files[5])$size > 0) {
	  log_data <- read.table(files[5], sep=" ", dec=".", header=F, fill=T)
	  ho_start = log_data[log_data[,2] == "HandoverStart", ]
	  ho_start = ho_start[ho_start[,3] == "UE", ]
	  ho_stop = log_data[log_data[,2] == "HandoverEndOk", ]
	  ho_stop = ho_stop[ho_stop[,3] == "UE", ]
	}
	# x_min
	# x_max
	# y_min
	# y_max
		
	png("buffer.png", height = 1080, width = 1920)
	plot(data[,1], data[,2], type="l", main="Buffer", xlab="Simulation Time [s]", ylab="Data [bytes]")
	# text(quality[,1], y_quality, labels=quality[,8]*100, pos=1, srt=90)
	# abline(v=quality[,1], lty=2, col="grey50")
	abline(v=ho_start[,1], col="skyblue")
	abline(v=ho_stop[,1], col="skyblue")
	dev.off()
	
	png("bw.png", height = 1080, width = 1920)
	plot(data[,1], ((data[,3]*8*1e-6)/(data[,1]-1)), type="o", main="Bandwidth", xlab="Simulation Time [s]", ylab="BW [mbit/s]")
	# text(quality[,1], y_quality, labels=quality[,8]*100, pos=1, srt=90)
	# abline(v=quality[,1], lty=2, col="grey50")
	abline(v=ho_start[,1], col="skyblue")
	abline(v=ho_stop[,1], col="skyblue")
  dev.off()

	png("buffer_time.png", height = 1080, width = 1920)
	plot(data[,1], data[,5], type="l", main="Buffer Time", xlab="Simulation Time [s]", ylab="Data [s]")
	lines(contr[,1], contr[,4], col="blue")
	lines(contr[,1], contr[,5], col="red")
	lines(contr[,1], contr[,6], col="green")
	lines(contr[,1], contr[,7], col="yellow")
	# text(quality[,1], y_quality, labels=quality[,8]*100, pos=1, srt=90)
	# abline(v=quality[,1], lty=2, col="grey50")
	abline(v=ho_start[,1], col="skyblue")
	abline(v=ho_stop[,1], col="skyblue")
  dev.off()

	png("play_transm_time.png", height = 1080, width = 1920)
	plot(data[,1], data[,6], type="l", main="Transmission and Playback Times", xlab="Simulation Time [s]", ylab="Data [s]")
	lines(data[,1], data[,7], col="blue")
	# text(quality[,1], y_quality, labels=quality[,8]*100, pos=1, srt=90)
	# abline(v=quality[,1], lty=2, col="grey50")
	abline(v=ho_start[,1], col="skyblue")
	abline(v=ho_stop[,1], col="skyblue")
  dev.off()

	png("play_transm_bytes.png", height = 1080, width = 1920)
	plot(data[,1], data[,3], type="l", main="Transmission and Playback Bytes", xlab="Simulation Time [s]", ylab="Data [bytes]")
	lines(data[,1], data[,4], col="blue")
	# text(quality[,1], y_quality, labels=quality[,8]*100, pos=1, srt=90)
	# abline(v=quality[,1], lty=2, col="grey50")
	abline(v=ho_start[,1], col="skyblue")
	abline(v=ho_stop[,1], col="skyblue")
  dev.off()

	png("play_transm_ratio.png", height = 1080, width = 1920)
	plot(ecdf(data[,7]/data[,6]), verticals=TRUE, main="Transmission and Playback Ratio", xlab="Played Video/Transmitted Video [%]", ylab="Frequency [%]")
	dev.off()


	# library(ggplot2)
	# df <- data.frame(data[,1], data[,5])
	# mydata <- qplot(df, aes(x="Simulation Time [s]", y="Data [s]")) + geom_line()
	# ggsave(mydata, file="ratings.pdf")

	setwd(wd)
}