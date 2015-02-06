// OpenCVDefault.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "device_launch_parameters.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;
using namespace cv;

Mat frameSnapshot;
Vec3b bgrPixel;
bool SnapshotMode;
int MouseX;
int MouseY;
Vec3b currentPixColor;
Mat frame;

bool is_file_exist(string fileName)
{
	ifstream infile(fileName.c_str());
	return infile.good();
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if  ( event == EVENT_RBUTTONDOWN && SnapshotMode) //Right click
	{
		bgrPixel = frameSnapshot.at<Vec3b>(Point(x, y));
		printf("Couleur sauvegardee\n");				
	}
	else if  ( event == EVENT_LBUTTONDOWN && SnapshotMode) //Left click
	{
		line(frameSnapshot, Point(x-10, y), Point(x+10, y), Scalar(bgrPixel[0], bgrPixel[1], bgrPixel[2]), 3,1);
		line(frameSnapshot, Point(x, y-10), Point(x, y+10), Scalar(bgrPixel[0], bgrPixel[1], bgrPixel[2]),3,1);
		vector<int> compression_params;
		compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
		compression_params.push_back(9);
		imwrite("snapshot.png", frameSnapshot, compression_params);
	}
	else if (event == EVENT_MOUSEMOVE && !SnapshotMode) //Mouse move
	{
		if (x >= 0 && x < frame.cols && y >= 0 && y < frame.rows)
		{
			MouseX = x;
			MouseY = y;
			currentPixColor = frame.at<Vec3b>(Point(x, y));
		}
	}
}

int main(int argc, char** argv)
{
	MouseX = 0;
	MouseY = 0;
	Mat edges;
	SnapshotMode = false;
	bgrPixel = (0,0,0);
	currentPixColor = (0,0,0);
	
		edges = imread("breadfish.png", CV_LOAD_IMAGE_COLOR);

		imshow("edges", edges);

		VideoCapture cap(0); // open the default camera
		if(!cap.isOpened())  // check if we succeeded
			return -1;
		frameSnapshot = Mat(480,640,CV_8UC3);
		frameSnapshot.setTo(0);
		frame = Mat(480,640,CV_8UC3);
		frame.setTo(0);
		int key=0;
		string path;
		bool fileLoaded=false;
		char Message[200];
		namedWindow("frame", 1);
		setMouseCallback("frame", CallBackFunc, NULL);

		while(key != 27)
		{
			cap >> frame; // get a new frame from camera
			memset(Message,0,200);
			sprintf(Message,"(%i, %i) = (%i, %i, %i)", MouseX, MouseY, currentPixColor[2], currentPixColor[1], currentPixColor[0]); //text out
			

			key = waitKey(30);

			if(key==102) // F key
			{
				cout << "Veuillez entrer le nom du fichier que vous desirez charger. (nom.extension)" << endl;
				getline(cin, path);
				if (is_file_exist(path))
				{
					frameSnapshot = imread(path, CV_LOAD_IMAGE_COLOR);
					cout << "Le fichier a ete charge." << endl;
					fileLoaded = true;
				}
				else
				{
					cout << "Le fichier na pas ete trouve." << endl;
					path="";
				}
			}

			if(key==13) //Enter key
			{
				vector<int> compression_params;
				compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
				compression_params.push_back(9);
				imwrite("snapshot.png", frame, compression_params);
				frame.copyTo(frameSnapshot);
			}	

			if (key==9 || key==13 || (key==102 && fileLoaded))
			{
				SnapshotMode = true;
				key=0;
				//mode image
				while(key != 9) //Tab key
				{
					key = waitKey(30);

					if(key==114)
					{
						line(frameSnapshot, Point(0,0), Point(frameSnapshot.cols, frameSnapshot.rows), cv::Scalar(0, 0, 200), 1, 1); 
					}

					if(key==32)
					{
						frameSnapshot.setTo(0);
					}

					imshow("frame", frameSnapshot);
				}
				SnapshotMode = false;
			}
			putText(frame, Message, Point(10,450), CV_FONT_HERSHEY_COMPLEX, 1,  cv::Scalar(0,0,200));
			imshow("frame", frame);
		}

	// the camera will be deinitialized automatically in VideoCapture destructor
	return 0;
}



