#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>  
#include <string.h>

static gboolean bus_call (GstBus     *bus,
                          GstMessage *msg,
                          gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

   case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;
    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;
 
      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }
  return TRUE;
}


int main (int   argc,
          char *argv[])
{
  GMainLoop *loop;

  GstElement *pipeline, *source,  *decoder, *rtpvp8depay, *sink;
  GstBus *bus;
  GstCaps* videoCaps ;

  /* Initialisation */
  gst_init (&argc, &argv);

 loop = g_main_loop_new (NULL, FALSE);

  /* Verification des arguments d'entree */
  if (argc != 4) {
    g_printerr ("Usage: %s <Ogg/Vorbis filename> IP port\n", argv[0]);
    return -1;
  }

  int port = atoi(argv[3]);
  char * ip = argv[2];
  char caps[1000] ;
  FILE *fp;
 
   if ((fp=fopen("./caps","r"))==NULL){
       fprintf(stderr,"Error! opening file caps, aborting... \n");
       return(-1);         
   }
   /*lecture du caps dans le fichier caps*/
   fscanf(fp,"%[^\n]", caps);
   fclose(fp);


    /* Creation des elements gstreamer */
    if(!(pipeline     = gst_pipeline_new ("video-player"))){
      g_printerr ("pipeline could not be created. Exiting.\n");
      return -1;
    }

    if(!(source       = gst_element_factory_make ("udpsrc",       "file-source"))) {
       g_printerr ("source could not created. Exiting.\n");
      return -1;
    }
    if(!(rtpvp8depay  = gst_element_factory_make ("rtpvp8depay",  "rtpvp8depay"))){
       g_printerr ("rtpvp8depay could not created. Exiting.\n");
      return -1;
    }
    if(!(decoder      = gst_element_factory_make ("vp8dec",     "vp8dec-decoder"))){
       g_printerr ("decoder could not created. Exiting.\n");
      return -1;
    }
   if(!( sink         = gst_element_factory_make ("autovideosink", "video-output"))){
       g_printerr ("sink could not created. Exiting.\n");
      return -1;
    }



  videoCaps = gst_caps_from_string(caps);



  /* Mise en place du pipeline */
  /* on configurer le nom du fichier a l'element source */
  g_object_set (G_OBJECT (source), "port", (gint)port, NULL);
  g_object_set (G_OBJECT (source), "address", ip , NULL);
  

  /* on rajoute une gestion de messages */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* on rajoute tous les elements dans le pipeline */
  gst_bin_add_many (GST_BIN (pipeline),
                    source, rtpvp8depay , decoder , sink, NULL);

  /* On relie les elements entre eux */
  /*ajout du caps entre rtpvp8depay et la source*/
  gst_element_link_filtered(source, rtpvp8depay , videoCaps);

  gst_caps_unref(videoCaps);

  gst_element_link (source, rtpvp8depay);
  gst_element_link_many (rtpvp8depay,decoder, sink, NULL);

  

  /* passage a l'etat "playing" du pipeline */
  g_print ("Lecture de : %s\n", argv[1]);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);

  /* Iteration */
  g_print ("En cours...\n");
  g_main_loop_run (loop);

  /* En dehors de la boucle principale, on nettoie proprement */
  g_print ("Arret de la lecture\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);
  g_print ("Suppression du pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  return 0;
} 
