# Master make file for TSE
#
# Michael Kilgore, April 2016

all: lib/cs50ds.a common/common.a crawler/crawler.o indexer/indexer.o querier/querier.o
# build the two librarys
lib/cs50ds.a: 
	cd ./lib; $(MAKE)
common/common.a:
	cd ./common; $(MAKE)
crawler/crawler.o:
	cd ./crawler; $(MAKE)
indexer/indexer.o:
	cd ./indexer; $(MAKE)
querier/querier.o:
	cd ./querier; $(MAKE)
clean:
	cd ./lib; $(MAKE) clean
	cd ./common; $(MAKE) clean
	cd ./crawler; $(MAKE) clean
	cd ./indexer; $(MAKE) clean
	cd ./querier; $(MAKE) clean