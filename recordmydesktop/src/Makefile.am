bin_PROGRAMS = recordmydesktop

recordmydesktop_SOURCES = \
	block_utils.c		\
	block_utils.h		\
	cache_audio.c		\
	cache_audio.h		\
	cache_frame.c		\
	cache_frame.h		\
	capture_sound.c		\
	capture_sound.h		\
	encode_cache.c		\
	encode_cache.h		\
	encode_image_buffer.c	\
	encode_image_buffer.h	\
	encode_sound_buffer.c	\
	encode_sound_buffer.h	\
	flush_to_ogg.c		\
	flush_to_ogg.h		\
	get_frame.c		\
	get_frame.h		\
	getzpixmap.c		\
	getzpixmap.h		\
	init_encoder.c		\
	init_encoder.h		\
	initialize_data.c	\
	initialize_data.h	\
	load_cache.c		\
	load_cache.h		\
	make_dummy_pointer.c	\
	make_dummy_pointer.h	\
	opendev.c		\
	opendev.h		\
	parseargs.c		\
	parseargs.h		\
	poll_events.c		\
	poll_events.h		\
	queryextensions.c	\
	queryextensions.h	\
	recordmydesktop.c	\
	rectinsert.c		\
	rectinsert.h		\
	register_callbacks.c	\
	register_callbacks.h	\
	rmd_cache.c		\
	rmd_cache.h		\
	rmd_error.c		\
	rmd_error.h		\
	rmd_frame.c		\
	rmd_frame.h		\
	rmd_jack.c		\
	rmd_jack.h		\
	rmd_rescue.c		\
	rmd_rescue.h		\
	rmd_timer.c		\
	rmd_timer.h		\
	rmdthreads.c		\
	rmdthreads.h		\
	rmdmacro.h		\
	rmdtypes.h		\
	setbrwindow.c		\
	setbrwindow.h		\
	shortcuts.c		\
	shortcuts.h		\
	skeleton.c		\
	skeleton.h		\
	specsfile.c		\
	specsfile.h		\
	update_image.c		\
	update_image.h		\
	yuv_utils.c		\
	yuv_utils.h		\
	wm_check.c		\
	wm_check.h

recordmydesktop_CPPFLAGS = -D_THREAD_SAFE -pthread -Wall
recordmydesktop_LDFLAGS  = @X_LIBS@ @X_EXTRA_LIBS@ @X_PRE_LIBS@


# RectInsert test
TESTS          = test-rectinsert
EXTRA_PROGRAMS = test-rectinsert
CLEANFILES     = $(EXTRA_PROGRAMS)

test_rectinsert_SOURCES = \
	test-rectinsert.c	\
	test-rectinsert-data.c	\
	test-rectinsert-data.h	\
	test-rectinsert-types.h	\
	rectinsert.c

test_rectinsert_CFLAGS = -Wall
