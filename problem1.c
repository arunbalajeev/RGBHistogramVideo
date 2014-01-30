#include<highgui.h>
#include<cv.h>
#include<stdlib.h>
IplImage* DrawHistogram(CvHistogram* hist,float xScale,float yScale)
{
  int i;
  float histMax;
  float nextValue;
  IplImage* imgHist=(IplImage*)cvCreateImage(cvSize(256*xScale,64*yScale),IPL_DEPTH_8U,1);
  cvZero(imgHist);
  cvGetMinMaxHistValue(hist,0,&histMax,NULL,NULL);
  for(i=0;i<256;i++)
  {
    float histValue=(float)cvQueryHistValue_1D(hist,i);
    if(i<255){float nextValue=cvQueryHistValue_1D(hist,i+1);}
    else {float nextValue=cvQueryHistValue_1D(hist,i);};
    CvPoint ptr1=cvPoint(i*xScale,64*yScale);
    CvPoint ptr2=cvPoint(i*xScale+xScale,64*yScale);
    CvPoint ptr3=cvPoint(i*xScale+xScale,(64-(nextValue*64/histMax)*yScale));
    CvPoint ptr4=cvPoint(i*xScale,(64-(histValue*64/histMax))*yScale);
    //printf("%d ",ptr3.x);
    CvPoint ptr[]={ptr1,ptr2,ptr3,ptr4,ptr1};
    int nptr=5;
    cvFillConvexPoly(imgHist,ptr,nptr,cvScalar(255,0,0,0),8,0); 
  }
  return imgHist;
}
IplImage* imposeImage(IplImage* imposed,IplImage* part,CvPoint loc,CvScalar S,CvScalar D)
{
  int i=0,j=0,k=0;
  for(i=0;i<part->height;i++)
  {
    for(j=0;j<part->width;j++)
    {
      CvScalar source=cvGet2D(imposed,i+loc.x,j+loc.y);
      CvScalar pt=cvGet2D(part,i,j);
      CvScalar merge;
      for(k=0;k<4;k++)
      {merge.val[k]=S.val[k]*source.val[k]+D.val[k]*pt.val[k];}
      //printf("%d %f %d are the numbers\n",j,merge.val[1],i);
      cvSet2D(imposed,i+loc.x,j+loc.y,merge);
    }
  }
  return imposed;
}
main()
{
  char name[]="Image Window";
  IplImage* img;
  CvFont font;
  cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,1.0,0.5,0.2,3,8);
  CvCapture* capture;
  int nbins=256;
  float range[]={0,255};
  float* ranges[]={ range };
  CvHistogram* histo;
  cvNamedWindow(name,CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Histogram Window1",CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Histogram Window2",CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Histogram Window3",CV_WINDOW_AUTOSIZE);
  cvNamedWindow("Imposed Image",CV_WINDOW_AUTOSIZE);
  capture=cvCreateCameraCapture(0);
  img=cvQueryFrame(capture);
  printf("%f %f are the values in the bins %d\n",range[1],range[0],img->width);
  int frameWidth=img->width;//capture,CV_CAP_PROP_FRAME_WIDTH);
  int frameHeight=img->height;//cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT);
  cvShowImage(name,img);
  printf("%f and %f are the numbers in ranges %d hai\n",&ranges[4],&ranges+6,frameWidth);
  IplImage* imgRed=cvCreateImage(cvSize(frameWidth,frameHeight),IPL_DEPTH_8U,1);
  IplImage* imgGreen=cvCreateImage(cvSize(frameWidth,frameHeight),IPL_DEPTH_8U,1);
  IplImage* imgBlue=cvCreateImage(cvSize(frameWidth,frameHeight),IPL_DEPTH_8U,1);
  IplImage* redHist=cvCreateImage(cvSize(256,64),IPL_DEPTH_8U,3);
  IplImage* greenHist=cvCreateImage(cvSize(256,64),IPL_DEPTH_8U,3);
  IplImage* blueHist=cvCreateImage(cvSize(256,64),IPL_DEPTH_8U,3);
  histo=cvCreateHist(1,&nbins,CV_HIST_ARRAY,ranges,1);
  cvClearHist(histo);
  IplImage* imgBlack=cvCreateImage(cvSize(256,64),IPL_DEPTH_8U,1);
  cvZero(imgBlack);
  printf("%d is the depth of imgBlack",img->depth);
  while(1)
  {
    img=cvQueryFrame(capture);
    cvShowImage(name,img);
    cvSplit(img,imgRed,imgGreen,imgBlue,NULL);
    cvCalcHist(&imgRed,histo,0,NULL);
    IplImage* imgRedHist=DrawHistogram(histo,1,1);
    cvClearHist(histo);
    cvCalcHist(&imgGreen,histo,0,NULL);
    IplImage* imgGreenHist=DrawHistogram(histo,1,1);
    cvClearHist(histo);
    cvCalcHist(&imgBlue,histo,0,NULL);
    IplImage* imgBlueHist=DrawHistogram(histo,1,1);
    cvClearHist(histo);
    cvZero(redHist);
    cvZero(greenHist);
    cvZero(blueHist);
    cvMerge(imgRedHist,imgBlack,imgBlack,NULL,redHist);
    cvMerge(imgBlack,imgGreenHist,imgBlack,NULL,greenHist);
    cvMerge(imgBlack,imgBlack,imgBlueHist,NULL,blueHist);
    cvShowImage("Histogram Window1",redHist);
    cvShowImage("Histogram Window2",greenHist);
    cvShowImage("Histogram Window3",blueHist);
    img=imposeImage(img,redHist,cvPoint(150,40),cvScalar(0.5,0.5,0.5,0.5),cvScalar(0.5,0.5,0.5,0.5));
    img=imposeImage(img,greenHist,cvPoint(250,40),cvScalar(0.5,0.5,0.5,0.5),cvScalar(0.5,0.5,0.5,0.5));
    img=imposeImage(img,blueHist,cvPoint(350,40),cvScalar(0.5,0.5,0.5,0.5),cvScalar(0.5,0.5,0.5,0.5));
    cvPutText(img,"BLUE",cvPoint(60,140),&font,cvScalar(255,0,0,0));
    cvPutText(img,"GREEN",cvPoint(60,240),&font,cvScalar(0,255,0,0));
    cvPutText(img,"RED",cvPoint(60,340),&font,cvScalar(0,0,255,0));
    cvShowImage("Imposed Image",img);
    if(cvWaitKey(30)==27)
    {break;}
  }
  cvWaitKey(0);
  cvReleaseCapture(&capture);
  cvDestroyWindow(name);
  cvDestroyWindow("Histogram Window1");
  cvDestroyWindow("Histogram Window2");
  cvDestroyWindow("Histogram Window3");
  cvDestroyWindow("Imposed Image");
}
