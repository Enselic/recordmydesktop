clean-local:
	@rm -rf _trial_temp

test:
	@make check -C flumotion/test

check-docs:
	@make check -C doc/reference

coverage:
	@trial --coverage coverage -R flumotion.test
	@$(PYTHON) common/show-coverage.py _trial_temp/coverage/flumotion.*

fixme:
	tools/fixme | less -R

release: dist
	make $(PACKAGE)-$(VERSION).tar.bz2.md5

# generate md5 sum files
%.md5: %
	md5sum $< > $@

# generate a sloc count
sloc:
	sloccount flumotion | grep "(SLOC)" | cut -d = -f 2

.PHONY: test
