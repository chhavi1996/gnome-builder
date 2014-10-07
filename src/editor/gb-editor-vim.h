/* gb-editor-vim.h
 *
 * Copyright (C) 2014 Christian Hergert <christian@hergert.me>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GB_EDITOR_VIM_H
#define GB_EDITOR_VIM_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GB_TYPE_EDITOR_VIM            (gb_editor_vim_get_type())
#define GB_TYPE_EDITOR_VIM_MODE       (gb_editor_vim_mode_get_type())
#define GB_EDITOR_VIM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GB_TYPE_EDITOR_VIM, GbEditorVim))
#define GB_EDITOR_VIM_CONST(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), GB_TYPE_EDITOR_VIM, GbEditorVim const))
#define GB_EDITOR_VIM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  GB_TYPE_EDITOR_VIM, GbEditorVimClass))
#define GB_IS_EDITOR_VIM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GB_TYPE_EDITOR_VIM))
#define GB_IS_EDITOR_VIM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GB_TYPE_EDITOR_VIM))
#define GB_EDITOR_VIM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  GB_TYPE_EDITOR_VIM, GbEditorVimClass))

typedef struct _GbEditorVim        GbEditorVim;
typedef struct _GbEditorVimClass   GbEditorVimClass;
typedef struct _GbEditorVimPrivate GbEditorVimPrivate;

typedef enum
{
  GB_EDITOR_VIM_NORMAL = 1,
  GB_EDITOR_VIM_INSERT,
  GB_EDITOR_VIM_COMMAND,
} GbEditorVimMode;

struct _GbEditorVim
{
  GObject parent;

  /*< private >*/
  GbEditorVimPrivate *priv;
};

struct _GbEditorVimClass
{
  GObjectClass parent_class;

  void (*begin_search)               (GbEditorVim *vim,
                                      const gchar *search_text);
  void (*command_visibility_toggled) (GbEditorVim *vim,
                                      gboolean     visibility);

  gpointer _padding1;
  gpointer _padding2;
  gpointer _padding3;
  gpointer _padding4;
  gpointer _padding5;
  gpointer _padding6;
};

GType            gb_editor_vim_get_type        (void) G_GNUC_CONST;
GType            gb_editor_vim_mode_get_type   (void) G_GNUC_CONST;
GbEditorVim     *gb_editor_vim_new             (GtkTextView     *text_view);
GbEditorVimMode  gb_editor_vim_get_mode        (GbEditorVim     *vim);
void             gb_editor_vim_set_mode        (GbEditorVim     *vim,
                                                GbEditorVimMode  mode);
const gchar     *gb_editor_vim_get_phrase      (GbEditorVim     *vim);
gboolean         gb_editor_vim_get_enabled     (GbEditorVim     *vim);
void             gb_editor_vim_set_enabled     (GbEditorVim     *vim,
                                                gboolean         enabled);
GtkWidget       *gb_editor_vim_get_text_view   (GbEditorVim     *vim);
gboolean         gb_editor_vim_execute_command (GbEditorVim     *vim,
                                                const gchar     *command);

G_END_DECLS

#endif /* GB_EDITOR_VIM_H */
