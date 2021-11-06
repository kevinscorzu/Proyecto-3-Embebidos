make
make install 
cd usr/
scp -r bin/ root@192.168.1.103:/usr/
scp -r include/ root@192.168.1.103:/usr/
scp -r lib/ root@192.168.1.103:/usr/
