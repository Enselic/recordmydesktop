bin_PROGRAMS = recordmydesktop

recordmydesktop_SOURCES = \
	cache_audio.c		\
	cache_frame.c		\
	capture_sound.c		\
	cleanup.c		\
	encode_cache.c		\
	encode_image_buffer.c	\
	encode_sound_buffer.c	\
	flush_to_ogg.c		\
	get_frame.c		\
	getzpixmap.c		\
	init_encoder.c		\
	initialize_data.c	\
	load_cache.c		\
	make_dummy_pointer.c	\
	opendev.c		\
	parseargs.c		\
	poll_events.c		\
	queryextensions.c	\
	recordmydesktop.c	\
	rectinsert.c		\
	rectinsert.h		\
	register_callbacks.c	\
	rmd_cache.c		\
	rmd_error.c		\
	rmd_frame.c		\
	rmd_jack.c		\
	rmd_rescue.c		\
	rmd_timer.c		\
	rmdthreads.c		\
	setbrwindow.c		\
	shortcuts.c		\
	skeleton.c		\
	specsfile.c		\
	update_image.c		\
	wm_check.c


INCLUDES = $(all_includes) -I$(top_srcdir)/include

recordmydesktop_LDFLAGS = @X_LIBS@ @X_EXTRA_LIBS@ @X_PRE_LIBS@
recordmydesktop_CFLAGS = -D_THREAD_SAFE -pthread -Wall


# RectInsert test
TESTS = test-rectinsert
EXTRA_PROGRAMS = test-rectinsert
CLEANFILES = $(EXTRA_PROGRAMS)

test_rectinsert_SOURCES = \
	test-rectinsert.c	\
	test-rectinsert-data.c	\
	test-rectinsert-data.h	\
	test-rectinsert-types.h	\
	rectinsert.c

test_rectinsert_CPPFLAGS = \
	-I$(top_srcdir)/include

test_rectinsert_CFLAGS = -Wall
