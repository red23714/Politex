all:
	qr_encoder qr_decoder

qr_encoder:
	gcc main.c qr_encoder.c -o qr_encoder -lqrencode 
qr_decoder:
	gcc qr_decoder.c -o qr_decoder -lzbar -lm

clear:
	rm qr_encoder 
	rm qr_decoder
