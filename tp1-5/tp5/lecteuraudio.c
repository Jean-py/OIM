#include <gst/gst.h> 
#include <glib.h>
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

//permet de connecter les entrées sortie, ne pas trop toucher apparement
static void
on_pad_added (GstElement *element,
              GstPad     *pad,
              gpointer    data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  /* We can now link this pad with the vorbis-decoder sink pad */
  g_print ("Dynamic pad created, linking demuxer/decoder\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");
 
  gst_pad_link (pad, sinkpad);
  gst_object_unref (sinkpad);
}

int main (int   argc,
          char *argv[])
{
  //déclaration des elements dont on a besoin, ajouter des pointeurs
  // correspondant au ellement que l'on veut
  GMainLoop *loop;
  GstElement *pipeline, *source, *demuxer, *decoder, *conv, *sink;
  /*theoradec ! videoscale ! video/x-raw-yuv, height=240, width=320 ! mpeg2enc !*/
  GstElement *theoradec,*videoscale,*queueAudio, *queueVideo , *sinkVideo;
  GstElement *volume , *videobalance;
  GstBus *bus;


  /* Initialisation */
  gst_init (&argc, &argv);


 loop = g_main_loop_new (NULL, FALSE);

  /* Verification des arguments d'entree */
  if (argc != 4) {
    g_printerr ("Usage: %s <Ogg/Vorbis filename> Noir/Blanc volume\n", argv[0]);
    return -1;
  }



  int volumeArg = atoi(argv[2]);
  int noirArg = atoi(argv[1]);


  /* Creation des elements gstreamer */
  //entre guillement on met une description
  pipeline   = gst_pipeline_new ("audio-player");
  source     = gst_element_factory_make ("filesrc",       "file-source");
  demuxer    = gst_element_factory_make ("oggdemux",      "ogg-demuxer");
  //vorbisdec (nom du truc gstreamer) appelé vorbis-decoder 
  decoder    = gst_element_factory_make ("vorbisdec",     "vorbis-decoder");
  conv       = gst_element_factory_make ("audioconvert",  "converter");
  sink       = gst_element_factory_make ("autoaudiosink", "audio-output"); 
  sinkVideo  = gst_element_factory_make ("autovideosink", "video-output");
  theoradec  = gst_element_factory_make ("theoradec", "video-decodeur");
  videoscale = gst_element_factory_make ("videoscale", "video-converteur");
  queueAudio = gst_element_factory_make ("queue", "queueAudio");
  queueVideo = gst_element_factory_make ("queue", "queueVideo");

  volume = gst_element_factory_make ("volume", "volume");
  videobalance = gst_element_factory_make ("videobalance", "videobalance");


  if (!volume || !videobalance || !pipeline || !source || !demuxer || !decoder || !conv || !sink || !theoradec || !videoscale || !queueVideo || !queueAudio) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }

  /* Mise en place du pipeline */
  /* on configurer le nom du fichier a l'element source */
  g_object_set (G_OBJECT (source), "location", argv[1], NULL);
  g_object_set (G_OBJECT (volume), "volume", volumeArg, NULL);
  g_object_set (G_OBJECT (videobalance), "saturation", noirArg, NULL);

  /* on rajoute une gestion de messages */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* on rajoute tous les elements dans le pipeline */
  /* file-source | ogg-demuxer | vorbis-decoder | converter | alsa-output */
  //pas d'ordre a utliser (il ne font partie du pipeline)
  gst_bin_add_many (GST_BIN (pipeline),volume,videobalance,source, demuxer, decoder, conv, sink,theoradec,videoscale,queueAudio, queueVideo,sinkVideo, NULL);

  /* On relie les elements entre eux */
  /* file-source -> ogg-demuxer ~> vorbis-decoder -> converter -> alsa-output */
  //source relié a demuxer
  gst_element_link (source, demuxer);
  //traitement sur l'audio ORDRE important
  gst_element_link_many (queueAudio,decoder, conv,volume, sink, NULL);
  gst_element_link_many (queueVideo,theoradec,videobalance, sinkVideo, NULL);
  //relie le demuxer au decoder
  g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), queueAudio);
  g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), queueVideo);


  /* Notez que le demuxer va etre lie au decodeur dynamiquement.
     la raison est que Ogg peut contenir plusieurs flux (par exemple
     audio et video). Les connecteurs sources seront crees quand la
     lecture debutera, par le demuxer quand il detectera le nombre et
     la nature des flux. Donc nous connectons une fonction de rappel
     qui sera execute quand le "pad-added" sera emis. */

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
