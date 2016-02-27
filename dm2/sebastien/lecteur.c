#include <gst/gst.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <stdlib.h>
#include <gst/interfaces/xoverlay.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>  // for GDK_WINDOW_XID

static gboolean playing=FALSE; // Determine si la video est en cours de lecture ou non
static gboolean SubsON=TRUE; // Determine si les sous-titres sont activés ou pas
static gulong video_window_xid = 0;
typedef struct CustomData
{
    GtkWidget *pProgress;
    GstElement *pipeline;
} CustomData;

static void on_destroy (GtkWindow *pWindow, gpointer data)
{
    GMainLoop *loop = (GMainLoop *) data;
    g_main_loop_quit (loop);
}

//Fonction appelée toute les 100 ms pour rafraichir la barre de progression
static gboolean refresh_Bar(gpointer data)
{
    gint64 pos, len;
    GstFormat fmt = GST_FORMAT_TIME;
    struct CustomData *cdata= (struct CustomData *) data;
    GstElement *pipeline=cdata->pipeline;
    GtkProgressBar *pProgress=(GtkProgressBar *) cdata->pProgress;
    gdouble fraction;

    if (gst_element_query_position (pipeline, &fmt, &pos) && gst_element_query_duration (pipeline, &fmt, &len))
    {
        fraction=(gdouble)pos / (gdouble)len;
        gtk_progress_bar_set_fraction(pProgress,fraction);
    }


    /* call me again */
    return TRUE;
}

static void on_play (GtkButton *button, gpointer pipeline)
{
    if (playing)
    {
        gst_element_set_state (pipeline, GST_STATE_PAUSED);
        gtk_button_set_label(button, "PLAY");
        playing=FALSE;
    }
    else
    {
        gst_element_set_state (pipeline, GST_STATE_PLAYING);
        gtk_button_set_label(button, "PAUSE");
        playing=TRUE;
    }
}

static int on_subs (GtkButton *button, gpointer subtitleoverlay)
{
    if (SubsON)
    {
        g_object_set (G_OBJECT (subtitleoverlay), "silent", TRUE, NULL);
        gtk_button_set_label(button, "Turn Subs ON");
        SubsON=FALSE;
    }
    else
    {
        g_object_set (G_OBJECT (subtitleoverlay), "silent", FALSE, NULL);
        gtk_button_set_label(button, "Turn Subs OFF");
        SubsON=TRUE;
    }
    return 0;
}

/* Fonction trouvée sur internet qui récupère l'id de la fenetre dans laquelle on mettra la video une fois créée */
static void video_widget_realize_cb (GtkWidget * widget, gpointer data)
{
#if GTK_CHECK_VERSION(2,18,0)
    // This is here just for pedagogical purposes, GDK_WINDOW_XID will call
    // it as well in newer Gtk versions
    if (!gdk_window_ensure_native (widget->window))
        g_error ("Couldn't create native window needed for GstXOverlay!");
#endif

#ifdef GDK_WINDOWING_X11
    video_window_xid = GDK_WINDOW_XID (gtk_widget_get_window (widget));
#endif
}


static gboolean bus_call (GstBus     *bus,
                          GstMessage *msg,
                          gpointer    data)
{
    GMainLoop *loop = (GMainLoop *) data;

    switch (GST_MESSAGE_TYPE (msg))
    {

    case GST_MESSAGE_EOS:
        g_print ("End of stream\n");
        g_main_loop_quit (loop);
        break;
    case GST_MESSAGE_ERROR:
    {
        gchar  *debug;
        GError *error;

        gst_message_parse_error (msg, &error, &debug);
        g_free (debug);

        g_printerr ("Error: %s\n", error->message);
        g_error_free (error);

        g_main_loop_quit (loop);
        break;
    }

    case GST_MESSAGE_ELEMENT:
    {
        /* Cas où le message annonce la création du flux video*/
        if (gst_structure_has_name (msg->structure, "prepare-xwindow-id"))
        {
            /* On vérifie que l'identifiant de la fenetre dans laquelle on veut mettre le flux vidéo à été initialisée */
            if (video_window_xid != 0)
            {
                GstXOverlay *xoverlay;

                // GST_MESSAGE_SRC (message) will be the video sink element
                xoverlay = GST_X_OVERLAY (GST_MESSAGE_SRC (msg));

                /* Cette fonction va envoyer le flux video récupéré dans xoverlay dans la fenêtre indentifiée par video_window_xid */
                gst_x_overlay_set_window_handle (xoverlay, video_window_xid);
            }
            else
            {
                g_warning ("Should have obtained video_window_xid by now!");
            }
        }
        break;
    }
    default:
        break;
    }
    return TRUE;
}

static void on_pad_added (GstElement *element, GstPad     *pad, gpointer    data)
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
    GtkWidget *pWindow, *video_window;
    GtkWidget *pVerticalBox, *pHorizontalBox;
    GtkWidget *buttonPlay_Pause, *buttonSubs;
    GtkWidget *pProgress;

    GMainLoop *loop;
    GstElement *pipeline, *source, *demuxer, *decoderAudio, *convAudio, *sinkAudio, *decoderVideo, *sinkVideo;
    GstElement *queueAudio, *queueVideo, *subtitleoverlay, *subparse, *sourceSubs;
    GstBus *bus;

    CustomData data;
    gint source_id;
    gboolean subs=FALSE; //Determine si l'utilisateur a entré un fichier de sous-titres

    /* Initialisation */
    gst_init (&argc, &argv);
    gtk_init (&argc, &argv);

    loop = g_main_loop_new (NULL, FALSE);

    /* Verification des arguments d'entree */
    if (argc != 3 && argc != 2)
    {
        g_printerr ("Usage: %s <Ogg/Vorbis filename> [<srt filename>]\n", argv[0]);
        return -1;
    }
    else if (argc == 3) subs=TRUE;

    /* Creation des elements gstreamer */
    pipeline        = gst_pipeline_new ("video-player");
    source          = gst_element_factory_make ("filesrc",       "file-source-video");
    demuxer         = gst_element_factory_make ("oggdemux",      "ogg-demuxer");

    decoderAudio    = gst_element_factory_make ("vorbisdec",     "vorbis-decoder");
    convAudio       = gst_element_factory_make ("audioconvert",  "audio-converter");
    sinkAudio       = gst_element_factory_make ("autoaudiosink", "audio-output");

    decoderVideo    = gst_element_factory_make ("theoradec", "theora-decoder");
    sinkVideo       = gst_element_factory_make ("autovideosink", "video-output");

    queueAudio      = gst_element_factory_make ("queue", "queueAudio");
    queueVideo      = gst_element_factory_make ("queue", "queueVideo");

    /* Création des element nécessaire à la gestion des sous-titres */
    if (subs)
    {
        subtitleoverlay = gst_element_factory_make ("subtitleoverlay", "subtitlepad");
        sourceSubs      = gst_element_factory_make ("filesrc",       "file-source-subs");
        subparse        = gst_element_factory_make ("subparse", "subparse");

        if (!pipeline || !source || !demuxer || !decoderAudio || !convAudio || !sinkAudio || !decoderVideo || !sinkVideo || !queueAudio

                || !queueVideo || !subtitleoverlay || !subparse || !sourceSubs)
        {
            g_printerr ("One element could not be created. Exiting.\n");
            return -1;
        }
    }
    else if (!pipeline || !source || !demuxer || !decoderAudio || !convAudio || !sinkAudio || !decoderVideo || !sinkVideo || !queueAudio || !queueVideo)
    {
        g_printerr ("One element could not be created. Exiting.\n");
        return -1;
    }

    //Création de la fenetre
    pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(pWindow), "Controls");
    gtk_window_set_default_size(GTK_WINDOW(pWindow), 900, 700);
    gtk_window_move(GTK_WINDOW(pWindow),0,0);
    g_signal_connect(G_OBJECT(pWindow), "destroy", G_CALLBACK(on_destroy), loop);

    /* Création de la boite contenant tous les elements */
    pVerticalBox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(pWindow), pVerticalBox);

    /* Création de la fenetre contenant la video*/
    video_window = gtk_drawing_area_new ();
    g_signal_connect (video_window, "realize", G_CALLBACK (video_widget_realize_cb), NULL);
    gtk_widget_set_double_buffered (video_window, FALSE);
    gtk_box_pack_start(GTK_BOX(pVerticalBox), video_window, TRUE, TRUE, 0);

    if (subs)
    {
        /*Création de la barre de progression */
        pProgress = gtk_progress_bar_new();
        gtk_box_pack_start(GTK_BOX(pVerticalBox), pProgress, False, TRUE, 0);

        /*Création de la boite contenant les deux boutons */
        pHorizontalBox = gtk_hbox_new(TRUE, 0);

        /*Creation du bouton play/pause */
        buttonPlay_Pause = gtk_button_new_with_label("PAUSE");
        gtk_box_pack_start(GTK_BOX(pHorizontalBox), buttonPlay_Pause, FALSE, TRUE, 0);
        g_signal_connect(G_OBJECT(buttonPlay_Pause), "clicked", G_CALLBACK(on_play), pipeline);

        /*Creation du bouton d'activation/désactivation des sous-titres */
        buttonSubs = gtk_button_new_with_label("Turn subs OFF");
        gtk_box_pack_start(GTK_BOX(pHorizontalBox), buttonSubs, FALSE, TRUE, 0);
        g_signal_connect(G_OBJECT(buttonSubs), "clicked", G_CALLBACK(on_subs), subtitleoverlay);
    }
    else //J'ai choisi d'avoir une apparence différente selon la présence ou non du bouton de gestion des sous-titres
    {
        /*Création de la boite contenant les deux boutons */
        pHorizontalBox = gtk_hbox_new(FALSE, 0);

        /*Creation du bouton play/pause */
        buttonPlay_Pause = gtk_button_new_with_label("PAUSE");
        gtk_box_pack_start(GTK_BOX(pHorizontalBox), buttonPlay_Pause, FALSE, TRUE, 0);
        g_signal_connect(G_OBJECT(buttonPlay_Pause), "clicked", G_CALLBACK(on_play), pipeline);

        /*Création de la barre de progression */
        pProgress = gtk_progress_bar_new();
        gtk_box_pack_start(GTK_BOX(pHorizontalBox), pProgress, TRUE, TRUE, 0);
    }
    gtk_box_pack_start(GTK_BOX(pVerticalBox), pHorizontalBox, FALSE, TRUE, 0);



    /* Mise en place du pipeline */
    /* on configurer le nom du fichier a l'element source */
    g_object_set (G_OBJECT (source), "location", argv[1], NULL);

    /* on rajoute une gestion de messages */
    bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
    gst_bus_add_watch (bus, bus_call, loop);
    gst_object_unref (bus);

    //Création du lecteur avec les sous-titres
    if (subs)
    {
        g_object_set (G_OBJECT (sourceSubs), "location", argv[2], NULL);

        /* on rajoute tous les elements dans le pipeline */
        gst_bin_add_many (GST_BIN (pipeline),
                          source, demuxer, decoderAudio, convAudio, sinkAudio, queueVideo, queueAudio, decoderVideo, sinkVideo, subtitleoverlay, subparse, sourceSubs, NULL);

        /* On relie les elements entre eux */
        gst_element_link (source, demuxer);
        gst_element_link (sourceSubs, subparse);
        gst_element_link (subparse, subtitleoverlay);
        gst_element_link_many (queueAudio, decoderAudio, convAudio, sinkAudio, NULL);
        gst_element_link_many (queueVideo, decoderVideo, subtitleoverlay, sinkVideo, NULL);
        g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), queueAudio);
        g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), queueVideo);
    }

    //Création du lecteur san les sou-titres
    else
    {
        /* on rajoute tous les elements dans le pipeline */
        gst_bin_add_many (GST_BIN (pipeline),
                          source, demuxer, decoderAudio, convAudio, sinkAudio, queueVideo, queueAudio, decoderVideo, sinkVideo, NULL);

        /* On relie les elements entre eux */
        gst_element_link (source, demuxer);
        gst_element_link_many (queueAudio, decoderAudio, convAudio, sinkAudio, NULL);
        gst_element_link_many (queueVideo, decoderVideo, sinkVideo, NULL);
        g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), queueAudio);
        g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), queueVideo);
    }
    /* Crée la fenetre de la video pour que l'on puisse récupérer son xid avant le lancement du pipeline */
    gtk_widget_realize (video_window);

    /* On vérifie que l'on a récupéré le xid de la fenetre */
    g_assert (video_window_xid != 0);

    /*On affiche la fenetre de controle */
    gtk_widget_show_all(pWindow);

    /* passage a l'etat "playing" du pipeline */
    g_print ("Lecture de : %s\n", argv[1]);
    gst_element_set_state (pipeline, GST_STATE_PLAYING);
    playing=TRUE;

    /* Appel de la fonction de rafraichissment de la barre de progression toutes les 100ms*/
    data.pProgress=pProgress;
    data.pipeline=pipeline;
    source_id = g_timeout_add(100, refresh_Bar, &data);

    /* Iteration */
    g_print ("En cours...\n");
    g_main_loop_run (loop);

    /* En dehors de la boucle principale, on nettoie proprement */
    g_print ("Arret de la lecture\n");
    gst_element_set_state (pipeline, GST_STATE_NULL);
    g_source_remove(source_id);
    g_print ("Suppression du pipeline\n");
    gst_object_unref (GST_OBJECT (pipeline));
    return 0;
}
