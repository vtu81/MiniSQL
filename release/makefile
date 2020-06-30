CC = g++ -std=c++11
OBJS = src/main.o src/Interpreter.o src/API.o src/Index_Manager.o src/Record_Manager.o src/Catalog_Manager.o src/Buffer_Manager.o src/SingleAttribute.o src/Condition.o

MiniSQL: $(OBJS)
	$(CC) $(OBJS) -o $@

clean:
	-rm $(OBJS) MiniSQL
