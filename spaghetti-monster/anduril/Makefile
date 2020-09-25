all:
	./build-all.sh

clean:
	rm -f *.hex *~ *.elf *.o

todo:
	@egrep 'TODO:|FIXME:' *.[ch]

models:
	@echo -n > MODELS
	@echo 'Model numbers:' >> MODELS
	@grep '^#define MODEL_NUMBER' cfg-*.h | perl -ne '/cfg-(.*)\.h:#define MODEL_NUMBER "(.*)"/ && print "$$2\t$$1\n";' | sort -n >> MODELS
	@echo 'Duplicates:' >> MODELS
	@cat cfg-*.h | grep '^#define MODEL_NUMBER' | sort | uniq -c | grep -v ' 1 ' || true >> MODELS
	@echo 'Missing:' >> MODELS
	@for f in cfg-*.h ; do grep --silent '^#define MODEL_NUMBER' $$f ; if [ "$$?" = "1" ] ; then echo "  $$f" ; fi ; done >> MODELS
	@cat MODELS

.phony: clean todo
