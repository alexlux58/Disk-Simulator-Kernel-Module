make clean
make
sudo insmod cidev.ko
sudo mknod /dev/cidev c 444 0
sudo chmod a+w /dev/cidev
