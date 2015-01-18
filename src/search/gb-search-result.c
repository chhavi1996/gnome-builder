/* gb-search-result.c
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

#include "gb-search-result.h"

struct _GbSearchResultPrivate
{
  gchar  *markup;
  gfloat  score;
};

G_DEFINE_TYPE_WITH_PRIVATE (GbSearchResult, gb_search_result, G_TYPE_OBJECT)

enum {
  PROP_0,
  PROP_MARKUP,
  PROP_SCORE,
  LAST_PROP
};

enum {
  ACTIVATE,
  LAST_SIGNAL
};

static GParamSpec *gParamSpecs [LAST_PROP];
static guint       gSignals [LAST_SIGNAL];

gint
gb_search_result_compare (const GbSearchResult *a,
                          const GbSearchResult *b)
{
  if (a->priv->score < b->priv->score)
    return -1;
  else if (a->priv->score > b->priv->score)
    return 1;
  else
    return 0;
}

GbSearchResult *
gb_search_result_new (const gchar *markup,
                      gfloat       score)
{
  return g_object_new (GB_TYPE_SEARCH_RESULT,
                       "markup", markup,
                       "score", score,
                       NULL);
}

const gchar *
gb_search_result_get_markup (GbSearchResult *result)
{
  g_return_val_if_fail (GB_IS_SEARCH_RESULT (result), NULL);

  return result->priv->markup;
}

static void
gb_search_result_set_markup (GbSearchResult *result,
                             const gchar    *markup)
{
  g_return_if_fail (GB_IS_SEARCH_RESULT (result));

  if (result->priv->markup != markup)
    {
      g_free (result->priv->markup);
      result->priv->markup = g_strdup (markup);
    }
}

gfloat
gb_search_result_get_score (GbSearchResult *result)
{
  g_return_val_if_fail (GB_IS_SEARCH_RESULT (result), 0.0f);

  return result->priv->score;
}

static void
gb_search_result_set_score (GbSearchResult *result,
                            gfloat          score)
{
  g_return_if_fail (GB_IS_SEARCH_RESULT (result));
  g_return_if_fail (score >= 0.0);
  g_return_if_fail (score <= 1.0);

  result->priv->score = score;
}

void
gb_search_result_activate (GbSearchResult *result)
{
  g_return_if_fail (GB_IS_SEARCH_RESULT (result));

  g_signal_emit (result, gSignals [ACTIVATE], 0);
}

static void
gb_search_result_finalize (GObject *object)
{
  GbSearchResultPrivate *priv = GB_SEARCH_RESULT (object)->priv;

  g_clear_pointer (&priv->markup, g_free);

  G_OBJECT_CLASS (gb_search_result_parent_class)->finalize (object);
}

static void
gb_search_result_get_property (GObject    *object,
                               guint       prop_id,
                               GValue     *value,
                               GParamSpec *pspec)
{
  GbSearchResult *self = GB_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_MARKUP:
      g_value_set_string (value, gb_search_result_get_markup (self));
      break;

    case PROP_SCORE:
      g_value_set_float (value, gb_search_result_get_score (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gb_search_result_set_property (GObject      *object,
                               guint         prop_id,
                               const GValue *value,
                               GParamSpec   *pspec)
{
  GbSearchResult *self = GB_SEARCH_RESULT (object);

  switch (prop_id)
    {
    case PROP_MARKUP:
      gb_search_result_set_markup (self, g_value_get_string (value));
      break;

    case PROP_SCORE:
      gb_search_result_set_score (self, g_value_get_float (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
gb_search_result_class_init (GbSearchResultClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = gb_search_result_finalize;
  object_class->get_property = gb_search_result_get_property;
  object_class->set_property = gb_search_result_set_property;

  gParamSpecs [PROP_MARKUP] =
    g_param_spec_string ("markup",
                         _("Markup"),
                         _("The pango markup to be rendered."),
                         NULL,
                         (G_PARAM_READWRITE |
                          G_PARAM_CONSTRUCT_ONLY |
                          G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class, PROP_MARKUP,
                                   gParamSpecs [PROP_MARKUP]);

  gParamSpecs [PROP_SCORE] =
    g_param_spec_float ("score",
                        _("Score"),
                        _("The result match score."),
                        0.0f,
                        1.0f,
                        0.0f,
                        (G_PARAM_READWRITE |
                         G_PARAM_CONSTRUCT_ONLY |
                         G_PARAM_STATIC_STRINGS));
  g_object_class_install_property (object_class, PROP_SCORE,
                                   gParamSpecs [PROP_SCORE]);

  gSignals [ACTIVATE] =
    g_signal_new ("activate",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GbSearchResultClass, activate),
                  NULL,
                  NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE,
                  0);
}

static void
gb_search_result_init (GbSearchResult *self)
{
  self->priv = gb_search_result_get_instance_private (self);
}
