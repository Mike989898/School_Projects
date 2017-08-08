#include <cairo.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>

/*
 
 Copyright (c) 2016 [CS50 pf50nffvtazrag]
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 If used in conjunction with a CS50 assignment/project, the file may not bundled in any
 public repository.
 
 */

void initialize_window(GtkWidget* window);
static void do_drawing(cairo_t *);
static void write_text(cairo_t *cr);
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);
static void do_drawing(cairo_t *cr);
static void draw_line(cairo_t *cr);
static void custom_background(cairo_t* cr);


int HEIGHT=811;
int WIDTH=1012;

cairo_surface_t* pic = NULL;

int main(int argc, char *argv[])
{
    //gtk_init(&argc, &argv);
    int dummyc=1;
    gtk_init(&dummyc, &argv);
    
    
    
    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    GtkWidget *darea;
    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);
    
    g_signal_connect(G_OBJECT(darea), "draw",G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy",G_CALLBACK(gtk_main_quit), NULL);
    
    initialize_window(window);
    

    gtk_main();
    
    cairo_surface_destroy(pic);

    
    return 0;
}


static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    do_drawing(cr);
    return FALSE;
}


static void do_drawing(cairo_t *cr)
{
    /*
     play with the uncommented lines and see what happens
     */
    write_text(cr);
    //    draw_line(cr);
    //    circle(cr);
    //    draw_rectangle(cr);
    //    triangle(cr);
}


void initialize_window(GtkWidget* window){
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "GTK window");
    gtk_widget_show_all(window);
}

static void custom_background(cairo_t* cr){
    
    if(!pic)pic =cairo_image_surface_create_from_png("a.png");
    
    cairo_set_source_surface(cr, pic, 0, 0);
    cairo_paint(cr);
    
}

static void write_text(cairo_t *cr){
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 20.0);
    
    cairo_move_to(cr, 200, 200);
    cairo_show_text(cr, "Hello World");
    
}


static void draw_line(cairo_t *cr){
    cairo_set_source_rgb(cr, 1, 0, 0);
    cairo_set_line_width(cr,10);
    
    cairo_move_to(cr, 10, 20);
    cairo_line_to(cr, 300, 20);
    cairo_stroke(cr); //important
    
}

static void circle(cairo_t* cr){
    
    cairo_set_line_width(cr, 9);
    cairo_set_source_rgb(cr, 0.69, 0.19, 0);
    
    
    cairo_translate(cr, 250, 250);
    cairo_arc(cr, 0, 0, 50, 0, 2 * 3.142);
    cairo_stroke_preserve(cr);
}

static void draw_rectangle(cairo_t* cr){
   // cairo_scale(cr,2,2);
    cairo_rectangle(cr, 20, 20, 120, 80);
    cairo_stroke_preserve(cr);
    
}

static void triangle(cairo_t* cr){
    cairo_move_to(cr,100,200);
    cairo_line_to(cr,100, 300);
    cairo_line_to(cr,200, 300);
    cairo_close_path(cr);
    
    cairo_move_to(cr,300,200);
    cairo_line_to(cr,300, 300);
    cairo_line_to(cr,400, 300);
    //cairo_close_path(cr);
    
    cairo_stroke_preserve(cr);
    
}

static void background(cairo_t* cr){
    cairo_surface_t* image = cairo_image_surface_create_from_png("a.png");
    cairo_set_source_surface(cr,image, 10, 10);
    cairo_paint(cr);
    
}


static void update(cairo_t *cr){
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, 20.0);
    
    cairo_move_to(cr, 700, 700);
    cairo_show_text(cr, "Hello World");
    
}



