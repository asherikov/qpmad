DATA_FOLDERS=oqp problems

fetch: ${DATA_FOLDERS}

oqp:
	${MAKE} fetch_qpoases
problems:
	${MAKE} fetch_qpoases

fetch_qpoases:
	svn export https://projects.coin-or.org/svn/qpOASES/misc/testingdata/cpp . --force

clean:

forceclean:
	rm -Rf ${DATA_FOLDERS}
