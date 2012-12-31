/*
 * Copyright (C) 2010 - Jeff Johnston
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <codeslayer/codeslayer.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gmodule.h>
#include <glib.h>

G_MODULE_EXPORT void activate          (CodeSlayer       *codeslayer);
G_MODULE_EXPORT void deactivate        (CodeSlayer       *codeslayer);
G_MODULE_EXPORT void configure         (CodeSlayer       *codeslayer);
                            
static void editor_added_action        (CodeSlayer       *codeslayer, 
                                        CodeSlayerEditor *editor);
static void editor_removed_action      (CodeSlayer       *codeslayer, 
                                        CodeSlayerEditor *editor);
static void editor_saved_action        (CodeSlayer       *codeslayer, 
                                        CodeSlayerEditor *editor);
static void editor_switched_action     (CodeSlayer       *codeslayer, 
                                        CodeSlayerEditor *editor);
static gchar* get_editor_basename      (CodeSlayerEditor *editor);
static void add_menu_bar_items          (CodeSlayer       *codeslayer);
static void remove_menu_bar_items       (CodeSlayer       *codeslayer);
static void remove_projects_tree_items (CodeSlayer       *codeslayer);
static void menu_bar_action             (GtkAction        *action, 
                                        CodeSlayer       *codeslayer);
static void add_projects_tree_items    (CodeSlayer       *codeslayer);
static void projects_tree_action       (GtkAction        *action, 
                                        GList            *selections,
                                        CodeSlayer       *codeslayer);
static void add_side_pane_widgets      (CodeSlayer       *codeslayer);
static void remove_side_pane_widgets   (CodeSlayer       *codeslayer);
static void add_bottom_pane_widgets    (CodeSlayer       *codeslayer);
static void remove_bottom_pane_widgets (CodeSlayer       *codeslayer);

static GtkWidget *menu_bar_item;                       
static GtkWidget *projects_tree_item;                   
static GtkWidget *side_pane_widget;                   
static GtkWidget *bottom_pane_widget;

static gulong added_handler_id;
static gulong removed_handler_id;
static gulong saved_handler_id;
static gulong switched_handler_id;

G_MODULE_EXPORT void
activate (CodeSlayer *codeslayer)
{
  gchar *folder_path;
  g_print ("Activate the spike plugin\n");

  add_menu_bar_items (codeslayer);
  add_projects_tree_items (codeslayer);
  add_side_pane_widgets (codeslayer);
  add_bottom_pane_widgets (codeslayer);
  
  folder_path = codeslayer_get_group_config_folder_path (codeslayer);
  g_print ("The group config folder path %s\n", folder_path);
  g_free (folder_path);

  added_handler_id = g_signal_connect (G_OBJECT (codeslayer), "editor-added", 
                                       G_CALLBACK (editor_added_action), NULL);

  removed_handler_id = g_signal_connect (G_OBJECT (codeslayer), "editor-removed", 
                                         G_CALLBACK (editor_removed_action), NULL);

  saved_handler_id = g_signal_connect (G_OBJECT (codeslayer), "editor-saved", 
                                       G_CALLBACK (editor_saved_action), NULL);

  switched_handler_id = g_signal_connect (G_OBJECT (codeslayer), "editor-switched", 
                                          G_CALLBACK (editor_switched_action), NULL);
}

G_MODULE_EXPORT void 
deactivate (CodeSlayer *codeslayer)
{
  g_print ("Deactivate the spike plugin\n");
  remove_menu_bar_items (codeslayer);
  remove_projects_tree_items (codeslayer);
  remove_side_pane_widgets (codeslayer);
  remove_bottom_pane_widgets (codeslayer);
  
  g_signal_handler_disconnect (codeslayer, added_handler_id);
  g_signal_handler_disconnect (codeslayer, removed_handler_id);
  g_signal_handler_disconnect (codeslayer, saved_handler_id);
  g_signal_handler_disconnect (codeslayer, switched_handler_id);
}

G_MODULE_EXPORT void 
configure (CodeSlayer *codeslayer)
{
  GtkWidget *dialog;
  g_print ("Configure the spike plugin\n");
  dialog = gtk_message_dialog_new (NULL,
                                   GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "It's paradise. Big windows, lovely gardens. It'll be perfect when we want the sunlight to kill us.");
  gtk_window_set_title (GTK_WINDOW (dialog), "CodeSlayer");
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void 
editor_added_action(CodeSlayer       *codeslayer, 
                    CodeSlayerEditor *editor) {
  gchar *basename = get_editor_basename (editor);
  g_print ("Added the %s editor\n", basename);
  g_free (basename);
}

static void 
editor_removed_action(CodeSlayer       *codeslayer, 
                      CodeSlayerEditor *editor) 
{
  gchar *basename = get_editor_basename (editor);
  g_print ("Removed the %s editor\n", basename);
  g_free (basename);
}

static void 
editor_saved_action(CodeSlayer       *codeslayer, 
                    CodeSlayerEditor *editor) 
{
  gchar *basename = get_editor_basename (editor);
  g_print ("Saved the %s editor \n", basename);
  g_free (basename);
}

static void 
editor_switched_action(CodeSlayer       *codeslayer, 
                       CodeSlayerEditor *editor) 
{
  gchar *basename = get_editor_basename (editor);
  g_print ("Switched to the %s editor\n", basename);
  g_free (basename);
}

static gchar* 
get_editor_basename (CodeSlayerEditor *editor)
{
  CodeSlayerDocument *document;
  document = codeslayer_editor_get_document (editor);
  return g_path_get_basename (codeslayer_document_get_file_path (document));
}

static void 
add_menu_bar_items (CodeSlayer *codeslayer) 
{
    GtkAccelGroup *accel_group;    
    accel_group = codeslayer_get_menu_bar_accel_group (codeslayer);
    
    menu_bar_item = gtk_menu_item_new_with_label ("Spike");
    gtk_widget_add_accelerator (menu_bar_item, "activate", 
                                accel_group, GDK_KEY_I, 
                                GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    codeslayer_add_to_menu_bar (codeslayer, GTK_MENU_ITEM (menu_bar_item));
    g_signal_connect(G_OBJECT (menu_bar_item), "activate", 
                               G_CALLBACK (menu_bar_action), codeslayer);
}

static void
remove_menu_bar_items (CodeSlayer *codeslayer) 
{
  codeslayer_remove_from_menu_bar (codeslayer, GTK_MENU_ITEM (menu_bar_item));
}

static void 
add_projects_tree_items (CodeSlayer *codeslayer) 
{
  projects_tree_item = codeslayer_menu_item_new_with_label ("Spike");
  codeslayer_add_to_projects_popup (codeslayer, GTK_MENU_ITEM (projects_tree_item));
  g_signal_connect(G_OBJECT (projects_tree_item), "projects-menu-selected", 
                             G_CALLBACK (projects_tree_action), codeslayer);
}

static void
remove_projects_tree_items (CodeSlayer *codeslayer) 
{
  codeslayer_remove_from_projects_popup (codeslayer, 
                                         GTK_MENU_ITEM (projects_tree_item));
}

static void 
menu_bar_action (GtkAction  *action, 
                CodeSlayer *codeslayer) 
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new (NULL,
                                   GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "Help me out here, Spock, I don't speak loser.");
  gtk_window_set_title (GTK_WINDOW (dialog), "CodeSlayer");
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void 
projects_tree_action (GtkAction  *action, 
                      GList      *selections,
                      CodeSlayer *codeslayer) 
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new (NULL,
                                   GTK_DIALOG_MODAL,
                                   GTK_MESSAGE_INFO,
                                   GTK_BUTTONS_OK,
                                   "So when do we destroy the world, already?");
  gtk_window_set_title (GTK_WINDOW (dialog), "CodeSlayer");
  gtk_dialog_set_default_response (GTK_DIALOG (dialog), GTK_RESPONSE_OK);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void 
add_side_pane_widgets (CodeSlayer *codeslayer)
{
  side_pane_widget = gtk_label_new ("It's a big rock. I can't wait to tell my friends.\n They don't have a rock this big.");
  codeslayer_add_to_side_pane (codeslayer, side_pane_widget, "Spike");
}

static void 
remove_side_pane_widgets (CodeSlayer *codeslayer)
{
  codeslayer_remove_from_side_pane (codeslayer, side_pane_widget);
}

static void 
add_bottom_pane_widgets (CodeSlayer *codeslayer)
{
  bottom_pane_widget = gtk_label_new ("She just left. She didn't even care enough to cut off my head or set me on fire.\n I mean, is that too much to ask? You know? Some little sign that she cared?");
  codeslayer_add_to_bottom_pane (codeslayer, bottom_pane_widget, "Spike");
}

static void 
remove_bottom_pane_widgets (CodeSlayer *codeslayer)
{
  codeslayer_remove_from_bottom_pane (codeslayer, bottom_pane_widget);
}
