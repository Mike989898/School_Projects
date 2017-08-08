#include <cairo.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>

void initialize_window(GtkWidget* window);
static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data);
static void do_drawing(cairo_t *cr);

static void custom_background(cairo_t* cr);
static gboolean time_handler(GtkWidget *widget);
int btwn(int x, int a, int b);
int collide(int x, int y, int x_2, int y_2);


typedef struct blip{
    cairo_surface_t* image;
    cairo_t* cr;
    float x; float y;
    int xd; int yd;
}blip_t;


typedef struct persist{
    int qindex;
    int qpersist;
    int qi;
    
}persist_t;

blip_t hanlons[5];
blip_t trumps[5];

int start = 0;
int HEIGHT=600;
int WIDTH=1012;

cairo_surface_t* pic = NULL;
cairo_pattern_t* pat = NULL;
persist_t persistent;
char* quote[4] = {"Make America Great Again!", "I'm dashing", "Very rich", "Build a wall"};



int main(int argc, char *argv[])
{

    //gtk_init(&argc, &argv);
    int dummyc=1;
    gtk_init(&dummyc, &argv);
    
    
    GtkWidget *window;
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    
    /* IMPORTANT, THIS IS THE WIDGET WE DRAW ON */
    GtkWidget *darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);
    
    /* ENSURE YOU CAPTURE THE draw signal on the Drawable Area! */
    g_signal_connect(G_OBJECT(darea), "draw",G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy",G_CALLBACK(gtk_main_quit), NULL);
    
    initialize_window(window);
    
    /*KEY ANIMATION LINE
     animate every 100 milliseconds
     time_handler is BOILER PLATE FUNCTION
     */
    g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);
    
    gtk_main();
    
    /* always destroy surface once you are done*/
    cairo_surface_destroy(pic);
    cairo_pattern_destroy(pat);

    
    return 0;
}


/*
TO ANIMATE YOU NEED THREE THINGS
A DRAWABLEAREA WIDGET, A TIMEOUT, AND A DRAW EVENT HANDLER

  g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);
  g_signal_connect(G_OBJECT(darea), "draw",G_CALLBACK(on_draw_event), NULL);
 
 MEAN THAT A DRAW EVENT SIGNAL EMMITED EVERY 1OO MILLISECONDS
 EVERYTIME SIGNAL EMMITED CALL on_draw_event
 
 First time draw_event called, it initializes stuff
 Thereafter, it moves stuff about
 
 
 */




void initialize_window(GtkWidget* window){
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER_ON_PARENT);
    gtk_window_set_default_size(GTK_WINDOW(window), 1012, 811);
    gtk_window_set_title(GTK_WINDOW(window), "GTK window");
    gtk_widget_show_all(window);
}

static void custom_background(cairo_t* cr){
    
    /* CREATE SURFACE FROM IMAGE */
    if(!pic)pic =cairo_image_surface_create_from_png("a.png");
    /* SET SURFACE TO OUR CURRENT WINDOW CONTEXT CR*/
    /* 0,0 IS THE LOCATION */
    cairo_set_source_surface(cr, pic, 0, 0);
    /* NEVER FORGET OT PAINT */
    cairo_paint(cr);
    
}

/* WHY ON EARTH AM I SHOUTING */

static void write_text_custom(cairo_t* cr, char* s, int x, int y, float size){
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    
    /* CREATE A PATTERN ONLY ONCE!! AVOID LEAKS*/
    if(!pat)pat=cairo_pattern_create_linear(0, 15, 0, 90*0.8);
    
    cairo_pattern_set_extend(pat, CAIRO_EXTEND_REPEAT);
    cairo_pattern_add_color_stop_rgb(pat, 0.0, 1, 0.6, 0);
    cairo_pattern_add_color_stop_rgb(pat, 0.5, 1, 0.3, 0);
    
    cairo_set_font_size(cr, size);
    cairo_move_to(cr, x, y);;
    cairo_text_path(cr, s);

    cairo_set_source(cr, pat);
    cairo_fill(cr);
}



static void load_teams(cairo_t* cr){
  
    persistent.qpersist = 0;

    /*
     Create an image from a surface only once!!
     Don't do it repeatedly or you get memory leaks!!
     WHICH IS WHY WE CALL LOAD TEAMS ONCE AND ONLY ONCE
     */
    cairo_surface_t* image = cairo_image_surface_create_from_png("hanlon.png");
    for(int i=0; i<5;i++){

        hanlons[i].cr = cr;
        hanlons[i].image = image;
        hanlons[i].x = i*100;
        hanlons[i].y=i*100;
        
        hanlons[i].xd =1;
        if(rand()%2) hanlons[i].xd =-1;
        
        hanlons[i].yd =1;
        if(rand()%2) hanlons[i].yd =-1;
    }
    

    
    image = cairo_image_surface_create_from_png("trump.png");
    for(int i=0; i<5;i++){
        trumps[i].cr = cr;
        trumps[i].image = image;
        trumps[i].x = 811-i*100;
        trumps[i].y=i*100;
        
        trumps[i].xd =3;
        if(rand()%2) trumps[i].xd =-3;
        
        hanlons[i].yd =3;
        if(rand()%2) trumps[i].yd =-3;

    }
    
}


/*
 CALLED WHEN DRAW EVENT SIGNAL IS RELEASED
 */
static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    do_drawing(cr);
    return FALSE;
}

static void update_coordinates(){
    /*
     Displace hanlon and trump blimps then check whether in bounds
     */
    for(int i=0; i<5;i++){
        hanlons[i].x += hanlons[i].xd;
        hanlons[i].y += hanlons[i].yd;
        
        if(hanlons[i].x > WIDTH-50 || hanlons[i].x <= 0) hanlons[i].xd *=-1;
        if(hanlons[i].x > WIDTH-50) hanlons[i].x = WIDTH-50;
        if(hanlons[i].x <= 0)hanlons[i].x = 0;
        
        if(hanlons[i].y > HEIGHT-50 || hanlons[i].y <= 0) hanlons[i].yd *=-1;
        if(hanlons[i].y > HEIGHT-50) hanlons[i].y = HEIGHT-50;
        if(hanlons[i].y <= 80)hanlons[i].y = 90;
    }
    
    
    for(int i=0; i<5;i++){
        trumps[i].x += trumps[i].xd;
        trumps[i].y += trumps[i].yd;
        
        if(trumps[i].x > WIDTH-50 || trumps[i].x <= 0) trumps[i].xd *=-1;
        if(trumps[i].x > WIDTH-50) trumps[i].x = WIDTH-50;
        if(trumps[i].x <= 0)trumps[i].x = 0;
        
        if(trumps[i].y > HEIGHT-50 || trumps[i].y <= 80) trumps[i].yd *=-1;
        if(trumps[i].y > HEIGHT-50) trumps[i].y = HEIGHT-50;
        if(trumps[i].y <= 80)trumps[i].y = 90;
        
    }
    
    //check for collisions
    for(int h=0; h<5; h++){
        for(int t=0; t<5;t++){
            if(!collide(hanlons[h].x, hanlons[h].y, trumps[t].x, trumps[t].y)) continue;
            
            blip_t* a= NULL;
            
            //randomy choose who changes direction
            if(rand()%2) a=&(trumps[t]);
            else a=&(hanlons[h]);
            
            a->xd*=-1;
            a->yd*=-1;
            
        }
        
    }

    
}

int collide(int x, int y, int x_2, int y_2){
    
    if(btwn(x_2,x,x+50) && btwn(y_2,y,y+50))return 1;
    
    if(btwn(x,x_2,x_2+50) && btwn(y,y_2,y_2+50))return 1;
    return 0;
}

int btwn(int x, int a, int b){
    return (a<=x && x<=b);
}






static void realtimedraw(){
    
    update_coordinates();

    for(int i=0; i<5;i++){
        cairo_set_source_surface(hanlons[i].cr,hanlons[i].image, hanlons[i].x, hanlons[i].y);
        cairo_paint(hanlons[i].cr);
    }
    
    for(int i=0; i<5;i++){
        cairo_set_source_surface(trumps[i].cr,trumps[i].image, trumps[i].x, trumps[i].y);
        cairo_paint(trumps[i].cr);
        
        if(persistent.qpersist){
            write_text_custom(trumps[0].cr, quote[persistent.qindex],trumps[persistent.qi].x, trumps[persistent.qi].y, 10.0);
            persistent.qpersist--;
        }
        else if(!(rand()%5) && rand()%2){
            int z=rand()%4;
            write_text_custom(trumps[0].cr, quote[z],trumps[i].x, trumps[i].y, 10.0);
            persistent.qpersist=6;
            persistent.qindex=z;
            persistent.qi=i;
        }
        
    }
    
    
}

//BOILER PLATE CALLBACK
static gboolean time_handler(GtkWidget *widget)
{
    gtk_widget_queue_draw(widget);
    return TRUE;
}

static void do_drawing(cairo_t *cr)
{
    

    custom_background(cr);
    if(!start){
        //INTIALIZE STUFF
        load_teams(cr);
        start++;
    } else {
        //MOVE STUFF
        realtimedraw();
    }

}

