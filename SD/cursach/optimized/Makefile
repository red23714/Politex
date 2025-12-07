all:
	qr_generator qr_decoder

qr_generator:
	gcc main.c qr_encoder.c -o qr_generator -lqrencode

qr_decoder:
	gcc qr_decoder.c -o qr_decoder -lzbar -lm

clear:
	rm qr_generator 
	rm qr_decoder
