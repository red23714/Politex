all:
	qr_generator qr_decoder

qr_generator:
	gcc main.c qr_encoder.c -o qr_generator -lqrencode

qr_decode:
	gcc qr_decoder.c -o qr_decoder -lzbar -lm
