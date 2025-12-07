make clear
make qr_generator qr_decoder
./qr_generator -c input.txt output.lzqr --password 1234
./qr_generator -d output.lzqr out.ppm --password 1234
./qr_decoder out.ppm 1234
