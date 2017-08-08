/*
 * Live map for MI game server
 * Opal team
 *
 * Based on gtk3 example program
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <gtk/gtk.h>

//static void print_hello (GtkWidget *widget,  gpointer   data) {
//  g_print ("Hello World\n");
//}


void dropPin(float lat, float lon, GtkFixed* fixed, bool drop) {

  //Coordinates of top left and bottom right of map
  float tlLon, tlLat, brLon, brLat;
  tlLat = 43.710447;
  tlLon = -72.309439;
  brLat = 43.697020;
  brLon = -72.265837;

  //Resolution of image, in pixels
  int width = 5312;
  int height = 2312;

  //Icon resolution, in pixels
  int iconWidth = 30;
  int iconHeight = 30;

  //Calculating where to place image
  int x = (int) floor((width * (lon - tlLon) /(brLon - tlLon)) - iconWidth/2);
  int y = (int) floor((height * (lat -tlLat) /(brLat - tlLat)) - iconHeight);

  //Choose the correct icon color depending on whether drop is true
  char* file;
  if (drop)
    file = "map/faLoc.png";
  else file = "map/cdLoc.png";

  GtkWidget* icon = gtk_image_new_from_file(file);
  gtk_fixed_put(fixed, icon, x, y);
}



static void activate (GtkApplication *app, gpointer user_data) {
  GtkWidget *window;
  GtkWidget* scrollable;
  GtkWidget* fixed;
  GtkWidget *button;
  GtkWidget *button_box;
  GtkWidget* background;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Mission Incomputable Map");
  gtk_window_set_default_size (GTK_WINDOW (window), 500, 500);

  scrollable = gtk_scrolled_window_new(NULL, NULL);
  //gtk_container_set_border_width(GTK_CONTAINER(scrollable), 2);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrollable), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_gesture_zoom_new(scrollable);

  fixed = gtk_fixed_new();
  background = gtk_image_new_from_file("map/map.png");
  gtk_container_add(GTK_CONTAINER(window), scrollable);
  gtk_container_add(GTK_CONTAINER(scrollable), fixed);
  gtk_fixed_put((GtkFixed*) fixed, background, 0, 0);
  dropPin(43.703357, -72.288552, (GtkFixed*) fixed, true);
  dropPin(43.710447, -72.309439, (GtkFixed*) fixed, false);
  dropPin(43.7050831248106, -72.29492692556658, (GtkFixed*) fixed, false);
  dropPin(43.70889161906508, -72.28428544564693, (GtkFixed*) fixed, false);
  dropPin(43.70647161906508, -72.28710544564693, (GtkFixed*) fixed, false);
  dropPin(43.70514073366537, -72.28813980827107, (GtkFixed*) fixed, false);
  dropPin(43.70248770250545, -72.28655459430716, (GtkFixed*) fixed, false);
  dropPin(43.70118770250545, -72.28733459430716, (GtkFixed*) fixed, false);


  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_fixed_put ((GtkFixed*) fixed, button_box, 0, 0);

  button = gtk_button_new_with_label ("Exit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
  gtk_container_add (GTK_CONTAINER (button_box), button);

  gtk_widget_show_all (window);




}



int openMap (void) {
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), 0, NULL);
  g_object_unref (app);

  return status;
}


void updateDrops(int drops) {

}

void updateAgents(int agents) {

}

void closeMap(void) {

}
