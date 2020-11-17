All : xml-dtd-validator

# xml.o : xml.c xml.h
# 	gcc -o xml.o -s xml.c xml.h

# menu.o : menu.c menu.h
# 	gcc -o menu.o -s menu.c menu.h
	
# main.o : main.c menu.c menu.h xml.c xml.h
# 	gcc -o main.o -s main.c menu.c menu.h xml.c xml.h

xml-dtd-validator : main.c definitions/menu.c definitions/*.c headers/*.h
	gcc -o xml-dtd-validator main.c definitions/*.c headers/*.h

clean :
	rm *.o

start :
	./xml-dtd-validator