args <- commandArgs(TRUE)
wd <- getwd()
names <- c()

library(ggplot2)
del <- c()
bw <- c()
loss <- c()
stops <- c()
stoptimes <- c()
graphdir <- wd

for (directory in args) {
	# get corresponding names in graph
	dirs <- c(unlist(strsplit(directory, "/")))
	
	graphdir <- dirs[-length(dirs)]
	graphdir <- paste(graphdir, collapse = "/")

	name <- tail(dirs, n=1)
	values <- c(unlist(strsplit(name, "_")))
	name <- paste(values[1], values[2], values[3], sep = "/")
	names <- c(names, name)
	values[1] = gsub("ms", "", values[1])
	values[2] = gsub("mbit", "", values[2])
	values[3] = gsub("pct", "", values[3])
	del <- c(del, as.numeric(values[1]))
	bw <- c(bw, as.numeric(values[2]))
	loss <- c(loss, as.numeric(values[3]))

	# do fancy stuff
	setwd (directory)
	files <- c("buffer.log", "linkconfig.txt", "framelist.txt", "controls.log")
	#data <- read.table(files[1], sep=" ", dec=".", header=F)
	contr <- read.table(files[4], sep=" ", dec=".", header=F)
	v_stop <- contr[contr[,2] == "stopPlay",]
	v_start <- contr[contr[,2] == "startPlay",]
	# there is always on "startPlay" at beginning of log
	v_start <- v_start[-1, ]
	
	# remove if there is one last "stopPlay" without corresponding "startPlay"
	if (nrow(v_stop) > nrow(v_start)) {
		v_stop <- v_stop[-nrow(v_stop), ]
	}

	stops <- c(stops, nrow(v_stop))
	stoptimes <- c(stoptimes, sum(v_start[,1] - v_stop[,1]))
	
	setwd(wd)
}

setwd(graphdir)
stopframe <- data.frame(names, del, bw, loss, stops, stoptimes)


ggplot(data=stopframe[stopframe$del == 0, ], aes(x=bw, y=stops)) + geom_point(size=2)
	# + geom_line() + theme(axis.text.x=element_text(angle = 90, hjust = 0))
ggsave("stops_bw.pdf", height=7, width=14)

ggplot(data=stopframe[stopframe$bw == 1000, ], aes(x=del, y=stops)) + geom_point(size=2)
	# + geom_line() + theme(axis.text.x=element_text(angle = 90, hjust = 0))
ggsave("stops_del.pdf", height=7, width=14)

ggplot(data=stopframe[stopframe$del == 0, ], aes(x=bw, y=stoptimes)) + geom_point(size=2)
	# + geom_line() + theme(axis.text.x=element_text(angle = 90, hjust = 0))
ggsave("stoptimes_bw.pdf", height=7, width=14)

ggplot(data=stopframe[stopframe$bw == 1000, ], aes(x=del, y=stoptimes)) + geom_point(size=2)
	# + geom_line() + theme(axis.text.x=element_text(angle = 90, hjust = 0))
ggsave("stoptimes_del.pdf", height=7, width=14)