gsettings_SCHEMAS = \
	org.gnome.builder.build.gschema.xml \
	org.gnome.builder.code-insight.gschema.xml \
	org.gnome.builder.editor.gschema.xml \
	org.gnome.builder.editor.language.gschema.xml \
	org.gnome.builder.extension-type.gschema.xml \
	org.gnome.builder.gschema.xml \
	org.gnome.builder.plugin.gschema.xml \
	org.gnome.builder.project-tree.gschema.xml \
	org.gnome.builder.terminal.gschema.xml \
	org.gnome.builder.workbench.gschema.xml \
	$(NULL)

.PRECIOUS: $(gsettings_SCHEMAS)

@GSETTINGS_RULES@

EXTRA_DIST = $(gsettings_SCHEMAS)

GITIGNOREFILES = \
	org.gnome.builder.code-insight.gschema.xml \
	$(NULL)

-include $(top_srcdir)/git.mk
