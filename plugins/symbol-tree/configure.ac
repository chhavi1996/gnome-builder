# --enable-symbol-tree-plugin=yes/no
AC_ARG_ENABLE([symbol-tree-plugin],
              [AS_HELP_STRING([--enable-symbol-tree-plugin=@<:@yes/no@:>@],
                              [Display a symbol tree for the current document.])],
              [enable_symbol_tree_plugin=$enableval],
              [enable_symbol_tree_plugin=yes])

# for if ENABLE_SYMBOL_TREE_PLUGIN in Makefile.am
AM_CONDITIONAL(ENABLE_SYMBOL_TREE_PLUGIN, test x$enable_symbol_tree_plugin != xno)

# Ensure our makefile is generated by autoconf
AC_CONFIG_FILES([plugins/symbol-tree/Makefile])
