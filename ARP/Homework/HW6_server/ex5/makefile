INC=-I headers
all: master server writer reader

master: 
	gcc $(INC) master.c utility/wrapfunc.c -o master 

server: 
	gcc $(INC) server.c utility/wrapfunc.c -o server -lncurses

writer: 
	gcc $(INC) writer.c utility/wrapfunc.c -o writer

reader: 
	gcc $(INC) reader.c utility/wrapfunc.c -o reader

# clean: 
# 	rm master server writer reader
