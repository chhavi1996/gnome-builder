/* gb-search-display-group.c
 *
 * Copyright (C) 2015 Christian Hergert <christian@hergert.me>
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

#include <glib/gi18n.h>

#include "gb-search-display-group.h"
#include "gb-search-provider.h"
#include "gb-search-result.h"
#include "gb-widget.h"

struct _GbSearchDisplayGroupPrivate
{
  /* References owned by instance */
  GbSearchProvider *provider;

  /* References owned by template */
  GtkLabel         *more_label;
  GtkListBoxRow    *more_row;
  GtkLabel         *label;
  GtkListBox       *rows;

  guint64           count;
};

G_DEFINE_TYPE_WITH_PRIVATE (GbSearchDisplayGroup,
                            gb_search_display_group,
                            GTK_TYPE_BOX)

enum {
  PROP_0,
  PROP_MAX_RESULTS,
  PROP_PROVIDER,
  PROP_SIZE_GROUP,
  LAST_PROP
};

enum {
  RESULT_ACTIVATED,
  RESULT_SELECTED,
  LAST_SIGNAL
};

static GQuark      gQuarkResult;
static GQuark      gQuarkRow;
static GParamSpec *gParamSpecs [LAST_PROP];
static guint       gSignals [LAST_SIGNAL];

GbSearchProvider *
gb_search_display_group_get_provider (GbSearchDisplayGroup *group)
{
  g_return_val_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group), NULL);

  return group->priv->provider;
}

static void
gb_search_display_group_set_provider (GbSearchDisplayGroup *group,
                                      GbSearchProvider     *provider)
{
  const gchar *verb;

  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));
  g_return_if_fail (!provider || GB_IS_SEARCH_PROVIDER (provider));

  if (provider)
    {
      group->priv->provider = g_object_ref (provider);
      verb = gb_search_provider_get_verb (provider);
      gtk_label_set_label (group->priv->label, verb);
    }
}

static void
gb_search_display_group_set_size_group (GbSearchDisplayGroup *group,
                                        GtkSizeGroup         *size_group)
{
  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));
  g_return_if_fail (!size_group || GTK_IS_SIZE_GROUP (size_group));

  if (size_group)
    gtk_size_group_add_widget (size_group, GTK_WIDGET (group->priv->label));
}

GtkWidget *
gb_search_display_group_create_row (GbSearchResult *result)
{
  GtkProgressBar *progress;
  GtkListBoxRow *row;
  const gchar *markup;
  GtkLabel *label;
  GtkBox *box;
  gfloat score;

  g_return_val_if_fail (GB_IS_SEARCH_RESULT (result), NULL);

  row = g_object_new (GTK_TYPE_LIST_BOX_ROW,
                      "visible", TRUE,
                      NULL);
  g_object_set_qdata_full (G_OBJECT (row), gQuarkResult,
                           g_object_ref (result), g_object_unref);
  g_object_set_qdata (G_OBJECT (result), gQuarkRow, row);
  box = g_object_new (GTK_TYPE_BOX,
                      "orientation", GTK_ORIENTATION_HORIZONTAL,
                      "spacing", 6,
                      "visible", TRUE,
                      NULL);
  gtk_container_add (GTK_CONTAINER (row), GTK_WIDGET (box));
  markup = gb_search_result_get_markup (result);
  label = g_object_new (GTK_TYPE_LABEL,
                        "hexpand", TRUE,
                        "label", markup,
                        "use-markup", TRUE,
                        "visible", TRUE,
                        "xalign", 0.0f,
                        "xpad", 6,
                        "ypad", 3,
                        NULL);
  gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (label));
  score = gb_search_result_get_score (result);
  progress = g_object_new (GTK_TYPE_PROGRESS_BAR,
                           "fraction", score,
                           "hexpand", FALSE,
                           "inverted", TRUE,
                           "visible", TRUE,
                           "width-request", 30,
                           "valign", GTK_ALIGN_CENTER,
                           "margin-start", 6,
                           "margin-end", 6,
                           NULL);
  gtk_style_context_add_class (
    gtk_widget_get_style_context (GTK_WIDGET (progress)), "osd");
  gtk_container_add (GTK_CONTAINER (box), GTK_WIDGET (progress));

  return GTK_WIDGET (row);
}

void
gb_search_display_group_remove_result (GbSearchDisplayGroup *group,
                                       GbSearchResult       *result)
{
  GtkWidget *row;

  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));
  g_return_if_fail (GB_IS_SEARCH_RESULT (result));

  row = g_object_get_qdata (G_OBJECT (result), gQuarkRow);

  if (row)
    gtk_container_remove (GTK_CONTAINER (group->priv->rows), row);
}

void
gb_search_display_group_add_result (GbSearchDisplayGroup *group,
                                    GbSearchResult       *result)
{
  GtkWidget *row;

  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));
  g_return_if_fail (GB_IS_SEARCH_RESULT (result));

  row = gb_search_display_group_create_row (result);
  gtk_container_add (GTK_CONTAINER (group->priv->rows), row);

  gtk_list_box_invalidate_sort (group->priv->rows);

  group->priv->count++;
}

void
gb_search_display_group_set_count (GbSearchDisplayGroup *group,
                                   guint64               count)
{
  GtkWidget *parent;
  gchar *markup;

  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));

  markup = g_strdup_printf (_("%"G_GUINT64_FORMAT" more"), count);
  gtk_label_set_label (group->priv->more_label, markup);
  g_free (markup);

  parent = GTK_WIDGET (group->priv->more_row);

  if ((count - group->priv->count) > 0)
    gtk_widget_show (parent);
  else
    gtk_widget_hide (parent);
}

static gint
compare_cb (GtkListBoxRow *row1,
            GtkListBoxRow *row2,
            gpointer       user_data)
{
  GtkListBoxRow *more_row = user_data;
  GbSearchResult *result1;
  GbSearchResult *result2;
  gfloat score1;
  gfloat score2;

  if (row1 == more_row)
    return 1;
  else if (row2 == more_row)
    return -1;

  result1 = g_object_get_qdata (G_OBJECT (row1), gQuarkResult);
  result2 = g_object_get_qdata (G_OBJECT (row2), gQuarkResult);

  score1 = gb_search_result_get_score (result1);
  score2 = gb_search_result_get_score (result2);

  if (score1 < score2)
    return 1;
  else if (score1 > score2)
    return -1;
  else
    return 0;
}

void
gb_search_display_group_unselect (GbSearchDisplayGroup *group)
{
  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));

  gtk_list_box_unselect_all (group->priv->rows);
}

static void
gb_search_display_group_row_activated (GbSearchDisplayGroup *group,
                                       GtkListBoxRow        *row,
                                       GtkListBox           *list_box)
{
  GbSearchResult *result;

  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));
  g_return_if_fail (!row || GTK_IS_LIST_BOX_ROW (row));
  g_return_if_fail (GTK_IS_LIST_BOX (list_box));

  result = g_object_get_qdata (G_OBJECT (row), gQuarkResult);
  g_signal_emit (group, gSignals [RESULT_ACTIVATED], 0, result);
}

static void
gb_search_display_group_row_selected (GbSearchDisplayGroup *group,
                                      GtkListBoxRow        *row,
                                      GtkListBox           *list_box)
{
  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));
  g_return_if_fail (!row || GTK_IS_LIST_BOX_ROW (row));
  g_return_if_fail (GTK_IS_LIST_BOX (list_box));

  if (row)
    {
      GbSearchResult *result;

      result = g_object_get_qdata (G_OBJECT (row), gQuarkResult);
      g_signal_emit (group, gSignals [RESULT_SELECTED], 0, result);
    }
}

void
gb_search_display_group_focus_first (GbSearchDisplayGroup *group)
{
  GtkListBoxRow *row;

  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));

  row = gtk_list_box_get_row_at_y (group->priv->rows, 1);

  if (row)
    {
      gtk_list_box_unselect_all (group->priv->rows);
      gtk_widget_child_focus (GTK_WIDGET (group->priv->rows), GTK_DIR_DOWN);
    }
}

void
gb_search_display_group_focus_last (GbSearchDisplayGroup *group)
{
  GtkAllocation alloc;
  GtkListBoxRow *row;

  g_return_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group));

  gtk_widget_get_allocation (GTK_WIDGET (group->priv->rows), &alloc);
  row = gtk_list_box_get_row_at_y (group->priv->rows, alloc.height - 2);

  if (row)
    {
      gtk_list_box_unselect_all (group->priv->rows);
      gtk_widget_child_focus (GTK_WIDGET (group->priv->rows), GTK_DIR_UP);
    }
}

static void
gb_search_display_group_header_cb (GtkListBoxRow *row,
                                   GtkListBoxRow *before,
                                   gpointer       user_data)
{
  g_return_if_fail (GTK_IS_LIST_BOX_ROW (row));

  if (row)
    {
      GtkWidget *header;

      header = g_object_new (GTK_TYPE_SEPARATOR,
                             "orientation", GTK_ORIENTATION_HORIZONTAL,
                             "visible", TRUE,
                             NULL);
      gtk_list_box_row_set_header (row, header);
    }
}

static gboolean
gb_search_display_group_keynav_failed (GbSearchDisplayGroup *group,
                                       GtkDirectionType      dir,
                                       GtkListBox           *list_box)
{
  gboolean ret = FALSE;

  g_return_val_if_fail (GB_IS_SEARCH_DISPLAY_GROUP (group), FALSE);
  g_return_val_if_fail (GTK_IS_LIST_BOX (list_box), FALSE);

  g_signal_emit_by_name (group, "keynav-failed", dir, &ret);

  return ret;
}

static void
gb_search_display_group_finalize (GObject *object)
{
  GbSearchDisplayGroupPrivate *priv = GB_SEARCH_DISPLAY_GROUP (object)->priv;

  g_clear_object (&priv->provider);

  G_OBJECT_CLASS (gb_search_display_group_parent_class)->finalize (object);
}

static void
gb_search_display_group_get_property (GObject    *object,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  GbSearchDisplayGroup *self = GB_SEARCH_DISPLAY_GROUP (object);

  switch (prop_id)
    {
    case PROP_PROVIDER:
      g_value_set_object (value, gb_search_display_group_get_provider (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gb_search_display_group_set_property (GObject      *object,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  GbSearchDisplayGroup *self = GB_SEARCH_DISPLAY_GROUP (object);

  switch (prop_id)
    {
    case PROP_PROVIDER:
      gb_search_display_group_set_provider (self, g_value_get_object (value));
      break;

    case PROP_SIZE_GROUP:
      gb_search_display_group_set_size_group (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gb_search_display_group_class_init (GbSearchDisplayGroupClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->finalize = gb_search_display_group_finalize;
  object_class->get_property = gb_search_display_group_get_property;
  object_class->set_property = gb_search_display_group_set_property;

  gParamSpecs [PROP_PROVIDER] =
    g_param_spec_object ("provider",
                         _("Provider"),
                         _("The search provider"),
                         GB_TYPE_SEARCH_PROVIDER,
                         (G_PARAM_READWRITE |
                          G_PARAM_CONSTRUCT_ONLY |
                          G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class, PROP_PROVIDER,
                                   gParamSpecs [PROP_PROVIDER]);

  gParamSpecs [PROP_SIZE_GROUP] =
    g_param_spec_object ("size-group",
                         _("Size Group"),
                         _("The size group for the label."),
                         GTK_TYPE_SIZE_GROUP,
                         (G_PARAM_WRITABLE |
                          G_PARAM_CONSTRUCT_ONLY |
                          G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class, PROP_SIZE_GROUP,
                                   gParamSpecs [PROP_SIZE_GROUP]);

  gSignals [RESULT_ACTIVATED] =
    g_signal_new ("result-activated",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL,
                  NULL,
                  g_cclosure_marshal_generic,
                  G_TYPE_NONE,
                  1,
                  GB_TYPE_SEARCH_RESULT);

  gSignals [RESULT_SELECTED] =
    g_signal_new ("result-selected",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  0,
                  NULL,
                  NULL,
                  g_cclosure_marshal_generic,
                  G_TYPE_NONE,
                  1,
                  GB_TYPE_SEARCH_RESULT);

  GB_WIDGET_CLASS_TEMPLATE (widget_class, "gb-search-display-group.ui");
  GB_WIDGET_CLASS_BIND (widget_class, GbSearchDisplayGroup, more_label);
  GB_WIDGET_CLASS_BIND (widget_class, GbSearchDisplayGroup, more_row);
  GB_WIDGET_CLASS_BIND (widget_class, GbSearchDisplayGroup, label);
  GB_WIDGET_CLASS_BIND (widget_class, GbSearchDisplayGroup, rows);

  gQuarkResult = g_quark_from_static_string ("GB_SEARCH_RESULT");
  gQuarkRow = g_quark_from_static_string ("GB_SEARCH_DISPLAY_ROW");
}

static void
gb_search_display_group_init (GbSearchDisplayGroup *self)
{
  self->priv = gb_search_display_group_get_instance_private (self);

  gtk_widget_init_template (GTK_WIDGET (self));

  gtk_list_box_set_sort_func (self->priv->rows, compare_cb,
                              self->priv->more_row, NULL);

  g_signal_connect_object (self->priv->rows,
                           "keynav-failed",
                           G_CALLBACK (gb_search_display_group_keynav_failed),
                           self,
                           G_CONNECT_SWAPPED);
  g_signal_connect_object (self->priv->rows,
                           "row-activated",
                           G_CALLBACK (gb_search_display_group_row_activated),
                           self,
                           G_CONNECT_SWAPPED);
  g_signal_connect_object (self->priv->rows,
                           "row-selected",
                           G_CALLBACK (gb_search_display_group_row_selected),
                           self,
                           G_CONNECT_SWAPPED);
  gtk_list_box_set_header_func (self->priv->rows,
                                gb_search_display_group_header_cb,
                                NULL, NULL);
}
