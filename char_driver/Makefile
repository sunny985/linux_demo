#sample driver module  
obj-m := char_demo.o
# uname -a 查看ubuntu的kernel版本，然后进行替换
KDIR = /lib/modules/3.13.0-117-generic/build/

all:  
	$(MAKE) -C $(KDIR) M=$(PWD)  

.PHONY:clean
clean:  
	rm -f *.mod.c *.mod.o *.ko *.o *.tmp_versions *.order *.symvers