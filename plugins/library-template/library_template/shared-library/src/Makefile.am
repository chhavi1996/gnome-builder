lib_LTLIBRARIES = lib{{name}}-@API_VERSION@.la

lib{{name_}}_@API_VERSION@_la_SOURCES = \
	{{prefix}}-version.h \
	{{name}}.h \
	$(NULL)

lib{{name_}}_@API_VERSION@_la_CFLAGS = \
	$({{NAME}}_CFLAGS) \
	$(WARN_CFLAGS) \
	$(NULL)
lib{{name_}}_@API_VERSION@_la_LIBADD = \
	$({{NAME}}_LIBS) \
	$(NULL)
lib{{name_}}_@API_VERSION@_la_LDFLAGS = \
	$(WARN_LDFLAGS) \
	$(NULL)

-include $(top_srcdir)/git.mk
