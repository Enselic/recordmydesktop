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
							rmd_cache.c

INCLUDES= $(all_includes) -I../include -I$x_includes

recordmydesktop_LDFLAGS = -D_THREAD_SAFE -pthread -Wall

