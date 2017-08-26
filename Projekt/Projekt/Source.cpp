#include <iostream>
#include <sstream>
#include <string>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>

// Do poprawnego dzialania najlepiej umiescic karty na jednolitym tle (sposob ich ulozenia nie ma znaczenia). Program potrafi czasami pomylic kolor. Glownie w skutek zbyt jasnego oswietlenia kart.
// Program rozpoznaje 5 kart po nacisnieciu spacji, gdy znajda sie w polu widzenia kamerki i wyswietla jaki to uklad pokerowy.

using namespace cv;
using namespace std;

bool calibration = false;
bool train = false;

const string camera_window = "Kamerka";
const string trackbarWindowName = "Trackbars";
const string threshold_window = "Threshold Image";

int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;

char key;
int Ace = 0;
int Two = 0;
int Three = 0;
int Four = 0;
int Five = 0;
int Six = 0;
int Seven = 0;
int Eight = 0;
int Nine = 0;
int Ten = 0;
int Jack = 0;
int Queen = 0;
int King = 0;

int Karo = 0;
int Kier = 0;
int Trefl = 0;
int Pik = 0;

string poker = "";

bool compareContourAreas(vector<Point> contour1, vector<Point> contour2);
void getCards(Mat im, int numcards);
vector<Point2f> getApprox(vector<Point2f> approx, Mat im);
vector<Point2f> approxFlip(vector<Point2f> test2, Mat im);
void whatCard(int index);
string pokerType();
void globalsClear();
void createTrackbars();

int main(int argc, char** argv) {
	Mat HSV;
	Mat frame;
	Mat threshold;
	Mat img;

	bool continueCapture = true;
	namedWindow(camera_window, WINDOW_AUTOSIZE);

	if (calibration == true) {
		createTrackbars();
	}

	VideoCapture cap(0);
	if (!cap.isOpened())
	{
		return -1;
	}
	while (continueCapture) {
		key = cvWaitKey(10);
		if (cap.read(frame)) {
			cap >> frame;
			if (calibration != true) {
				putText(frame, poker, Point(300, 430), 1, 2, Scalar(0, 0, 255), 2);
				imshow(camera_window, frame);
				if (char(key) == 32) {
					imwrite("camera.jpg", frame);
					img = imread("camera.jpg", 1);
					getCards(img, 5);
					poker = pokerType();
					globalsClear();
				}
			}
			if (calibration == true) {
				namedWindow(trackbarWindowName, 0);
				imshow(camera_window, frame);
				if (char(key) == 32) {
					imwrite("camera.jpg", frame);
					img = imread("camera.jpg", 1);
					namedWindow("karta", 1);
					imshow("karta", img);
					getCards(img, 1);
					globalsClear();
				}
				Mat img1 = imread("wkolorze.jpg", 1);
				cvtColor(img1, HSV, COLOR_BGR2HSV);
				inRange(HSV, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), threshold);
				namedWindow(threshold_window, WINDOW_AUTOSIZE);
				imshow(threshold_window, threshold);
			}
		}
	}

	return(0);
}

void createTrackbars() {

	namedWindow(trackbarWindowName, WINDOW_AUTOSIZE);
	char TrackbarName[50];

	sprintf(TrackbarName, "H_MIN", H_MIN);
	sprintf(TrackbarName, "H_MAX", H_MAX);
	sprintf(TrackbarName, "S_MIN", S_MIN);
	sprintf(TrackbarName, "S_MAX", S_MAX);
	sprintf(TrackbarName, "V_MIN", V_MIN);
	sprintf(TrackbarName, "V_MAX", V_MAX);

	createTrackbar("H_MIN", trackbarWindowName, &H_MIN, H_MAX);
	createTrackbar("H_MAX", trackbarWindowName, &H_MAX, H_MAX);
	createTrackbar("S_MIN", trackbarWindowName, &S_MIN, S_MAX);
	createTrackbar("S_MAX", trackbarWindowName, &S_MAX, S_MAX);
	createTrackbar("V_MIN", trackbarWindowName, &V_MIN, V_MAX);
	createTrackbar("V_MAX", trackbarWindowName, &V_MAX, V_MAX);


}

// zrodlo - https://stackoverflow.com/questions/13495207/opencv-c-sorting-contours-by-their-contourarea
bool compareContourAreas(vector<Point> contour1, vector<Point> contour2) {
	double i = fabs(contourArea(Mat(contour1)));
	double j = fabs(contourArea(Mat(contour2)));
	return (i > j);
}
// koniec zrodla

void getCards(Mat im, int numcards) {
	Mat src;
	Mat im1 = im;
	cvtColor(im, im, COLOR_BGR2GRAY);
	imwrite("getCards_1.jpg", im);
	GaussianBlur(im, im, Size(1, 1), 1000);
	imwrite("getCards_2.jpg", im);
	threshold(im, im, 120, 255, THRESH_BINARY);
	imwrite("getCards_3.jpg", im);



	src = im;

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	// zrodlo - https://stackoverflow.com/questions/13495207/opencv-c-sorting-contours-by-their-contourarea
	// find contours
	findContours(im, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));
	// sort contours
	sort(contours.begin(), contours.end(), compareContourAreas);
	// grab contours
	// koniec zrodla

	Mat perspective, perspective2, perspective3, perspective4;
	int licznik = 0;
	vector<Point2f> test;
	test.push_back(Point2f(float(0), float(0))); //1
	test.push_back(Point2f(float(0), float(599))); //2
	test.push_back(Point2f(float(449), float(599))); //3
	test.push_back(Point2f(float(449), float(0))); //4
	Mat aaa = imread("temp.jpg", 1);
	long long int s = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s_min;
	int temp_min_index;
	//zrodlo - http://docs.opencv.org/2.4/modules/imgproc/doc/structural_analysis_and_shape_descriptors.html?highlight=drawcontours#drawcontours
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		//koniec zrodla
		if (idx <= numcards - 1) {
			Mat dst = Mat::zeros(src.rows, src.cols, CV_8UC3);
			vector<Point2f> approx, approx2;
			string nazwa = "im" + to_string(idx + 1);
			Scalar color(255, 255, 0);
			//drawContours(dst, contours, idx, Scalar(255, 255, 0), 1, 8, hierarchy);
			//zrodlo - https://github.com/abidrahmank/OpenCV2-Python/blob/master/OpenCV_Python_Blog/sudoku_v_0.0.6/sudoku.py
			approxPolyDP(contours[idx], approx, 0.02*arcLength(contours[idx], true), true);
			//koniec zrodla
			approx = getApprox(approx, im);
			approx2 = approxFlip(approx, im);
			//zrodlo - https://github.com/abidrahmank/OpenCV2-Python/blob/master/OpenCV_Python_Blog/sudoku_v_0.0.6/sudoku.py
			warpPerspective(im, perspective, getPerspectiveTransform(approx, test), Size(450, 600));
			warpPerspective(im, perspective2, getPerspectiveTransform(approx2, test), Size(450, 600));
			// koniec zrodla (zrodlo odnosi sie do samej metody zastosowania gdyz u mnie nieco sie to rozni)
			imwrite("testoviron.jpg", perspective);
			imwrite("testoviron2.jpg", perspective2);
			//zrodlo - https://github.com/abidrahmank/OpenCV2-Python/blob/master/OpenCV_Python_Blog/sudoku_v_0.0.6/sudoku.py
			warpPerspective(im1, perspective3, getPerspectiveTransform(approx, test), Size(450, 600));
			warpPerspective(im1, perspective4, getPerspectiveTransform(approx2, test), Size(450, 600));
			//koniec zrodla (zrodlo odnosi sie do samej metody zastosowania gdyz u mnie nieco sie to rozni)
			imwrite("wkolorze.jpg", perspective3);
			imwrite("wkolorze1.jpg", perspective4);
			Mat im2 = imread("wkolorze.jpg", 1);
			Mat im3 = imread("wkolorze1.jpg", 1);
			Mat HSV, HSV1, threshold_red, threshold_black, threshold_red1, threshold_black1;
			cvtColor(im2, HSV, COLOR_BGR2HSV);
			cvtColor(im3, HSV1, COLOR_BGR2HSV);
			/*inRange(HSV, Scalar(0, 0, 0), Scalar(256, 102, 256), threshold_red);
			inRange(HSV, Scalar(0, 0, 126), Scalar(221, 256, 256), threshold_black);
			inRange(HSV1, Scalar(0, 0, 0), Scalar(256, 102, 256), threshold_red1);
			inRange(HSV1, Scalar(0, 0, 126), Scalar(221, 256, 256), threshold_black1);
			imwrite("threshold_red.jpg", threshold_red);
			imwrite("threshold_black.jpg", threshold_black);
			imwrite("threshold_red1.jpg", threshold_red1);
			imwrite("threshold_black1.jpg", threshold_black1);*/
			if (train != true) {
				//algorytm porownujacy karty
				long long int temp_min = cv::sum(aaa)[0];
				for (int i = 0; i <= 51; i++) {
					string nazwa2 = "im" + to_string(i);
					Mat diff, temp, diff2;
					Mat diff_red, diff_black, diff_red1, diff_black1;
					temp = imread(nazwa2 + ".jpg", 1);
					Mat testoviron = imread("testoviron.jpg", 1);
					Mat testoviron2 = imread("testoviron2.jpg", 1);
					absdiff(temp, testoviron, diff);
					absdiff(temp, testoviron2, diff2);
					threshold_red = imread("threshold_red.jpg", 1);
					threshold_black = imread("threshold_black.jpg", 1);
					threshold_red1 = imread("threshold_red1.jpg", 1);
					threshold_black1 = imread("threshold_black1.jpg", 1);
					/*absdiff(temp, threshold_red, diff_red);
					absdiff(temp, threshold_black, diff_black);
					absdiff(temp, threshold_red1, diff_red1);
					absdiff(temp, threshold_black1, diff_black1);*/
					//
					s = cv::sum(diff)[0];
					s1 = cv::sum(diff2)[0];
					/*s2 = cv::sum(diff_red)[0];
					s3 = cv::sum(diff_black)[0];
					s4 = cv::sum(diff_red1)[0];
					s5 = cv::sum(diff_black1)[0];*/

					/*long long int s6[6] = {s, s1, s2, s3, s4, s5};
					sort(s6, s6 + 2);
					s_min = s6[0];*/

					if (s1 < s) {
						s = s1;
					}
					if (s < temp_min) {
						temp_min = s;
						temp_min_index = i;
					}
				}
				//cout << "im" << temp_min_index << endl;
				whatCard(temp_min_index);
				////////////
			}
		}

	}
}

vector<Point2f> getApprox(vector<Point2f> approx, Mat im) {
	vector<Point2f> pomoc;
	pomoc.push_back(approx[0]);
	pomoc.push_back(approx[1]);
	pomoc.push_back(approx[2]);
	pomoc.push_back(approx[3]);
	approx.clear();

	double x0 = 0, x1 = 0, x2 = 0, x3 = 0, y0 = 0, y1 = 0, y2 = 0, y3 = 0;
	double line1, line2, line3;
	float x_min = 999999;
	float y_min = 999999;
	int x_min_iteration, y_equal_iteration, y_min_iteration, x_equal_iteration;
	int fora = 0;

	//zrodlo - https://stackoverflow.com/questions/33358634/access-array-values-in-a-vectorpoint2f-and-separate-x-and-y-coordinates
	for (auto i = pomoc.begin(); i != pomoc.end(); ++i) {
		double x_value = i->x;
		double y_value = i->y;
		//koniec zrodla
		if (fora == 0) {
			x0 = x_value;
			y0 = y_value;
		}
		if (fora == 1) {
			x1 = x_value;
			y1 = y_value;
		}
		if (fora == 2) {
			x2 = x_value;
			y2 = y_value;
		}
		if (fora == 3) {
			x3 = x_value;
			y3 = y_value;
			line1 = sqrt((pow(x0 - x1, 2)) + (pow(y0 - y1, 2)));
			line2 = sqrt((pow(x0 - x2, 2)) + (pow(y0 - y2, 2)));
			line3 = sqrt((pow(x0 - x3, 2)) + (pow(y0 - y3, 2)));
			if (line1 < line3 && line3 < line2) {
				approx.push_back(pomoc[3]);
				approx.push_back(pomoc[0]);
				approx.push_back(pomoc[1]);
				approx.push_back(pomoc[2]);
			}
			if (line3 < line1 && line1 < line2) {
				approx.push_back(pomoc[0]);
				approx.push_back(pomoc[1]);
				approx.push_back(pomoc[2]);
				approx.push_back(pomoc[3]);
			}


		}
		fora++;


	}

	return approx;


}

vector<Point2f> approxFlip(vector<Point2f> approx2, Mat im) {
	vector<Point2f> pomoc;
	pomoc.push_back(approx2[0]);
	pomoc.push_back(approx2[1]);
	pomoc.push_back(approx2[2]);
	pomoc.push_back(approx2[3]);
	approx2.clear();

	approx2.push_back(pomoc[2]);
	approx2.push_back(pomoc[3]);
	approx2.push_back(pomoc[0]);
	approx2.push_back(pomoc[1]);
	return approx2;
}

void whatCard(int index) {
	//0 - 12 trefl
	//13 - 25 pik
	//26 - 38 kier
	//39 - 51 karo
	string card;

	if (0 <= index && index <= 12) {
		Trefl = Trefl + 1;
		//cout << "Trefl" << endl;
	}
	else if (13 <= index && index <= 25) {
		Pik = Pik + 1;
		//cout << "Pik" << endl;
	}
	else if (26 <= index && index <= 38) {
		Kier = Kier + 1;
		//cout << "Kier" << endl;
	}
	else if (39 <= index && index <= 51) {
		Karo = Karo + 1;
		//cout << "Karo" << endl;
	}
	if (index == 0 || index == 13 || index == 26 || index == 39) {
		Ace = Ace + 1;
		return;
	}
	else if (index == 1 || index == 14 || index == 27 || index == 40) {
		Two = Two + 1;
		return;
	}
	else if (index == 2 || index == 15 || index == 28 || index == 41) {
		Three = Three + 1;
		return;
	}
	else if (index == 3 || index == 16 || index == 29 || index == 42) {
		Four = Four + 1;
		return;
	}
	else if (index == 4 || index == 17 || index == 30 || index == 43) {
		Five = Five + 1;
		return;
	}
	else if (index == 5 || index == 18 || index == 31 || index == 44) {
		Six = Six + 1;
		return;
	}
	else if (index == 6 || index == 19 || index == 32 || index == 45) {
		Seven = Seven + 1;
		return;
	}
	else if (index == 7 || index == 20 || index == 33 || index == 46) {
		Eight = Eight + 1;
		return;
	}
	else if (index == 8 || index == 21 || index == 34 || index == 47) {
		Nine = Nine + 1;
		return;
	}
	else if (index == 9 || index == 22 || index == 35 || index == 48) {
		Ten = Ten + 1;
		return;
	}
	else if (index == 10 || index == 23 || index == 36 || index == 49) {
		Jack = Jack + 1;
		return;
	}
	else if (index == 11 || index == 24 || index == 37 || index == 50) {
		Queen = Queen + 1;
		return;
	}
	else if (index == 12 || index == 25 || index == 38 || index == 51) {
		King = King + 1;
		return;
	}

}

string pokerType() {
	int Ace1, Two1, Three1, Four1, Five1, Six1, Seven1, Eight1, Nine1, Ten1, Jack1, Queen1, King1;
	int value[13] = { Ace, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King };
	int color[4] = { Trefl, Pik, Kier, Karo };
	sort(value, value + 13);
	sort(color, color + 4);

	int color1 = color[3];

	int biggest = value[12];
	int secondBiggest = value[11];

	if (color1 == 5) {
		if (value[12] == 1 && value[11] == 1 && value[10] == 1 && value[9] == 1 && value[8] == 1) {
			if (Ace == 1 && King == 1 && Queen == 1 && Jack == 1 && Ten == 1) {
				poker = "Poker Krolewski";
				return poker;
			}
			else if (King == 1 && Queen == 1 && Jack == 1 && Ten == 1 && Nine == 1) {
				poker = "Poker";
				return poker;
			}
			else if (Queen == 1 && Jack == 1 && Ten == 1 && Nine == 1 && Eight == 1) {
				poker = "Poker";
				return poker;
			}
			else if (Jack == 1 && Ten == 1 && Nine == 1 && Eight == 1 && Seven == 1) {
				poker = "Poker";
				return poker;
			}
			else if (Ten == 1 && Nine == 1 && Eight == 1 && Seven == 1 && Six == 1) {
				poker = "Poker";
				return poker;
			}
			else if (Nine == 1 && Eight == 1 && Seven == 1 && Six == 1 && Five == 1) {
				poker = "Poker";
				return poker;
			}
			else if (Eight == 1 && Seven == 1 && Six == 1 && Five == 1 && Four == 1) {
				poker = "Poker";
				return poker;
			}
			else if (Seven == 1 && Six == 1 && Five == 1 && Four == 1 && Three == 1) {
				poker = "Poker";
				return poker;
			}
			else if (Six == 1 && Five == 1 && Four == 1 && Three == 1 && Two == 1) {
				poker = "Poker";
				return poker;
			}
		}
		else {
			poker = "Kolor";
			return poker;
		}
	}

	else if (biggest == 4) {
		poker = "Kareta";
		return poker;
	}
	else if (biggest == 3) {
		if (secondBiggest == 2) {
			poker = "Full";
			return poker;
		}
		else {
			poker = "Trojka";
			return poker;
		}
	}
	else if (biggest == 2) {
		if (secondBiggest == 2) {
			poker = "Dwie pary";
			return poker;
		}
		else {
			poker = "Para";
			return poker;
		}
	}
	else if (value[12] == 1 && value[11] == 1 && value[10] == 1 && value[9] == 1 && value[8] == 1) {

		if (Ace == 1 && King == 1 && Queen == 1 && Jack == 1 && Ten == 1) {
			poker = "Strit";
			return poker;
		}
		else if (King == 1 && Queen == 1 && Jack == 1 && Ten == 1 && Nine == 1) {
			poker = "Strit";
			return poker;
		}
		else if (Queen == 1 && Jack == 1 && Ten == 1 && Nine == 1 && Eight == 1) {
			poker = "Strit";
			return poker;
		}
		else if (Jack == 1 && Ten == 1 && Nine == 1 && Eight == 1 && Seven == 1) {
			poker = "Strit";
			return poker;
		}
		else if (Ten == 1 && Nine == 1 && Eight == 1 && Seven == 1 && Six == 1) {
			poker = "Strit";
			return poker;
		}
		else if (Nine == 1 && Eight == 1 && Seven == 1 && Six == 1 && Five == 1) {
			poker = "Strit";
			return poker;
		}
		else if (Eight == 1 && Seven == 1 && Six == 1 && Five == 1 && Four == 1) {
			poker = "Strit";
			return poker;
		}
		else if (Seven == 1 && Six == 1 && Five == 1 && Four == 1 && Three == 1) {
			poker = "Strit";
			return poker;
		}
		else if (Six == 1 && Five == 1 && Four == 1 && Three == 1 && Two == 1) {
			poker = "Strit";
			return poker;
		}
	}
	else {
		poker = "Brak ukladu";
		return poker;
	}
}

void globalsClear() {
	Ace = 0;
	Two = 0;
	Three = 0;
	Four = 0;
	Five = 0;
	Six = 0;
	Seven = 0;
	Eight = 0;
	Nine = 0;
	Ten = 0;
	Jack = 0;
	Queen = 0;
	King = 0;

	Karo = 0;
	Kier = 0;
	Trefl = 0;
	Pik = 0;
}
