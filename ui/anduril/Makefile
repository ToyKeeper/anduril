all:
	./build-all.sh

clean:
	rm -f *.hex *~ *.elf *.o

todo:
	@egrep 'TODO:|FIXME:' *.[ch]

models:
	@./models.py > MODELS
	@cat MODELS

.phony: clean todo
