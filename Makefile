all:
	make -f Makefile_bmp clean
	make -f Makefile_bmp
	make -f Makefile_jpg clean
	make -f Makefile_jpg
	make -f Makefile_png clean
	make -f Makefile_png
clean:
	make -f Makefile_bmp clean
	make -f Makefile_jpg clean
	make -f Makefile_png clean
