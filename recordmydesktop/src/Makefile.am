bin_PROGRAMS = recordmydesktop



recordmydesktop_SOURCES=	recordmydesktop.c\
							getzpixmap.c\
							parseargs.c\
							rectinsert.c\
							setbrwindow.c\
							queryextensions.c\
							register_callbacks.c\
							get_frame.c\
							update_image.c\
							poll_damage.c\
							encode_image_buffer.c\
							bgr_to_yuv.c\
							flush_to_ogg.c\
							make_dummy_pointer.c\
							opendev.c\
							capture_sound.c\
							encode_sound_buffer.c\
							init_encoder.c\
							cache_frame.c\
							cache_audio.c\
							rmd_cache.c\
							load_cache.c\
							wm_check.c\
							encode_cache.c\
							rmdthreads.c\
							initialize_data.c\
							rmd_jack.c\
							cleanup.c\
							specsfile.c

INCLUDES= $(all_includes) -I$(top_srcdir)/include

recordmydesktop_LDFLAGS = @X_LIBS@ @X_EXTRA_LIBS@ @X_PRE_LIBS@
recordmydesktop_CFLAGS = -D_THREAD_SAFE -pthread -Wall
