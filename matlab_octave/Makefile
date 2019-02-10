octave: forceclean configure
	octave --no-history --silent make.m

matlab: forceclean configure
	matlab -nosplash -nodesktop -nojvm -r 'make; quit' | tail +10


octave_test: octave_onlineqp octave_basic

octave_basic:
	octave --no-history --silent test_basic.m

octave_onlineqp:
	git submodule update --init
	cd onlineqp_collection; ${MAKE} fetch
	octave --no-history --silent test_onlineqp.m


configure:
	cp config.h ../include/qpmad/


clean:
	rm -f *.o

forceclean: clean
	rm -f *.mex*
