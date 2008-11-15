bin_PROGRAMS = recordmydesktop

recordmydesktop_SOURCES = \
	rmd.c \
	rmd_block_utils.c \
	rmd_block_utils.h \
	rmd_cache.c \
	rmd_cache.h \
	rmd_cache_audio.c \
	rmd_cache_audio.h \
	rmd_cache_frame.c \
	rmd_cache_frame.h \
	rmd_capture_sound.c \
	rmd_capture_sound.h \
	rmd_encode_cache.c \
	rmd_encode_cache.h \
	rmd_encode_image_buffer.c \
	rmd_encode_image_buffer.h \
	rmd_encode_sound_buffer.c \
	rmd_encode_sound_buffer.h \
	rmd_error.c \
	rmd_error.h \
	rmd_flush_to_ogg.c \
	rmd_flush_to_ogg.h \
	rmd_frame.c \
	rmd_frame.h \
	rmd_get_frame.c \
	rmd_get_frame.h \
	rmd_getzpixmap.c \
	rmd_getzpixmap.h \
	rmd_init_encoder.c \
	rmd_init_encoder.h \
	rmd_initialize_data.c \
	rmd_initialize_data.h \
	rmd_jack.c \
	rmd_jack.h \
	rmd_load_cache.c \
	rmd_load_cache.h \
	rmd_macro.h \
	rmd_make_dummy_pointer.c \
	rmd_make_dummy_pointer.h \
	rmd_opendev.c \
	rmd_opendev.h \
	rmd_parseargs.c \
	rmd_parseargs.h \
	rmd_poll_events.c \
	rmd_poll_events.h \
	rmd_queryextensions.c \
	rmd_queryextensions.h \
	rmd_rectinsert.c \
	rmd_rectinsert.h \
	rmd_register_callbacks.c \
	rmd_register_callbacks.h \
	rmd_rescue.c \
	rmd_rescue.h \
	rmd_setbrwindow.c \
	rmd_setbrwindow.h \
	rmd_shortcuts.c \
	rmd_shortcuts.h \
	rmd_specsfile.c \
	rmd_specsfile.h \
	rmd_threads.c \
	rmd_threads.h \
	rmd_timer.c \
	rmd_timer.h \
	rmd_types.h \
	rmd_update_image.c \
	rmd_update_image.h \
	rmd_wm_check.c \
	rmd_wm_check.h \
	rmd_wm_is_compositing.c \
	rmd_wm_is_compositing.h \
	rmd_yuv_utils.c \
	rmd_yuv_utils.h \
	skeleton.c \
	skeleton.h

recordmydesktop_CPPFLAGS = -D_THREAD_SAFE -pthread -Wall
recordmydesktop_LDFLAGS  = @X_LIBS@ @X_EXTRA_LIBS@ @X_PRE_LIBS@


# RectInsert test
TESTS          = test-rectinsert
EXTRA_PROGRAMS = test-rectinsert
CLEANFILES     = $(EXTRA_PROGRAMS)

test_rectinsert_SOURCES = \
	test-rectinsert.c \
	test-rectinsert-data.c \
	test-rectinsert-data.h \
	test-rectinsert-types.h \
	rmd_rectinsert.c

test_rectinsert_CFLAGS = -Wall
