all:
	qr_encoder qr_decoder

qr_encoder:
	gcc main.c qr_encoder.c lzw.c -o qr_encoder -lqrencode -O3 -march=native -mtune=native -flto -funroll-loops 

qr_decoder:
	gcc qr_decoder.c -o qr_decoder -lzbar -lm

clean:
	rm qr_encoder 
	rm qr_decoder
