make clear
make qr_encoder qr_decoder
./qr_encoder -c input.txt output.lzqr --password 1234
./qr_encoder -d output.lzqr out.ppm --password 1234
./qr_decoder out.ppm 1234
