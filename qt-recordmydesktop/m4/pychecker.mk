# include this snippet for the pychecker stuff
# Makefile.am needs to define
# PYCHECKER_WHITELIST
# and
# PYCHECKER_BLACKLIST

# we redirect stderr so we don't get messages like
# warning: couldn't find real module for class SSL.Error (module name: SSL)
# which can't be turned off in pychecker
pycheckersplit:
	@echo running pychecker on each file ...
	@for file in $(filter-out $(PYCHECKER_BLACKLIST),$(wildcard $(PYCHECKER_WHITELIST))); \
	do \
		PYTHONPATH=`pwd`					\
		pychecker -Q -F misc/pycheckerrc 			\
                `ls $(top_srcdir)/misc/setup.py 2> /dev/null`		\
                `ls $(top_srcdir)/misc/pycheckerhelp.py	2> /dev/null` 	\
		$$file			 				\
		> /dev/null 2>&1;					\
		if test $$? -ne 0; then 				\
			echo "Error on $$file";				\
			pychecker -Q -F misc/pycheckerrc		\
        		`ls $(top_srcdir)/misc/setup.py	2> /dev/null`	\
                	`ls $(top_srcdir)/misc/pycheckerhelp.py	2> /dev/null` 	\
			$$file;	break; fi; 				\
	done

pychecker:
	@echo running pychecker ...
	@PYTHONPATH=`pwd`					\
	pychecker -Q -F misc/pycheckerrc			\
        `ls $(top_srcdir)/misc/setup.py	2> /dev/null`		\
        `ls $(top_srcdir)/misc/pycheckerhelp.py	2> /dev/null` 	\
	$(filter-out $(PYCHECKER_BLACKLIST),$(wildcard $(PYCHECKER_WHITELIST)))\
		2> /dev/null || make pycheckerverbose

pycheckerverbose:
	@echo running pychecker ...
	PYTHONPATH=`pwd`					\
	pychecker -F misc/pycheckerrc				\
        `ls $(top_srcdir)/misc/setup.py	2> /dev/null`	\
        `ls $(top_srcdir)/misc/pycheckerhelp.py	2> /dev/null` 	\
	$(filter-out $(PYCHECKER_BLACKLIST),$(wildcard $(PYCHECKER_WHITELIST)))
