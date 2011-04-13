/* Copyright (c) 2011 Dmitri Bachtin <d.bachtin@gmail.com>

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define HAARCASCADE_FRONTALFACE_XML "haarcascade_frontalface_alt.xml"

#define FACELOCK_DBG_WINDOW_NAME "debug window"
#define FACELOCK_DBG_WAITKEY_TIMEOUT 100

#define LOCK_COMMAND "gnome-screensaver-command -l"

static CvCapture *cam = NULL;
static IplImage *frame = NULL;
static CvHaarClassifierCascade *cascade = NULL;
static CvMemStorage *storage = NULL;

void init_camera (void);
void init_face_detection(void);
void facelockd_atexit (void);
int get_num_of_detected_faces (IplImage *frame);
void main_loop (void);
void lock_screen (void);

#ifdef FACELOCK_DBG
void init_debug_window (void);
#endif

int main (void)
{
  atexit (&facelockd_atexit);
  init_camera ();
  init_face_detection ();
  
#ifdef FACELOCK_DBG
  init_debug_window ();
#endif

  main_loop ();

  return 0;
}

void init_camera (void)
{
  cam = cvCaptureFromCAM (0);
  if (NULL == cam)
    {
      fprintf (stderr, "Could not open a webcam device\n");
      exit (1);
    }
}

void main_loop (void)
{
  int num_faces = 0;
#ifdef FACELOCK_DBG
  int key = 0;
  while (key != 'q')
#else
  while (1)
#endif
    {
      frame = cvQueryFrame (cam);
      assert (frame != NULL);
      if (NULL == frame)
	  break;

      num_faces = get_num_of_detected_faces (frame);
      if (num_faces != 1)
	lock_screen ();

#ifdef FACELOCK_DBG
      cvShowImage (FACELOCK_DBG_WINDOW_NAME, frame);
      key = cvWaitKey(FACELOCK_DBG_WAITKEY_TIMEOUT);
#endif
    }
}

void facelockd_atexit (void)
{
#ifdef FACELOCK_DBG
  cvDestroyWindow (FACELOCK_DBG_WINDOW_NAME);
#endif
  if (NULL != cam)
    cvReleaseCapture (&cam);

  if (NULL != cascade)
    cvReleaseHaarClassifierCascade (&cascade);

  if (NULL != storage)
    cvReleaseMemStorage (&storage);
}

#ifdef FACELOCK_DBG
void init_debug_window (void)
{
  cvNamedWindow (FACELOCK_DBG_WINDOW_NAME, CV_WINDOW_AUTOSIZE);
}
#endif

int get_num_of_detected_faces (IplImage *frame)
{
  CvSeq *faces;
#ifdef FACELOCK_DBG
  int i;
#endif

  faces = cvHaarDetectObjects (frame,
			       cascade,
			       storage,
			       1.1,
			       3,
			       0,
			       cvSize (40, 40));
  
#ifdef FACELOCK_DBG
  for (i = 0; i < (faces ? faces->total : 0); ++i)
    {
      CvRect *r = (CvRect*) cvGetSeqElem (faces, i);
      cvRectangle (frame,
		   cvPoint (r->x, r->y),
		   cvPoint (r->x + r->width, r->y + r->height),
		   CV_RGB (255, 0, 0),
		   1, 8, 0);
    }
#endif

  return faces->total;
}

void init_face_detection(void)
{
  cascade = cvLoad (HAARCASCADE_FRONTALFACE_XML, 0, 0, 0);
  storage = cvCreateMemStorage (0);
}

void lock_screen (void)
{
  system (LOCK_COMMAND);
  exit (0);
}
