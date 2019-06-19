// This file is base on the framework of OpenMVG, an Open Multiple View Geometry C++ library.
//进行匹配结果显示以及adr验证算法
#include "openMVG/image/image_io.hpp"
#include "openMVG/image/image_concat.hpp"
#include "openMVG/features/svg_features.hpp"
#include "openMVG/matching/regions_matcher.hpp"
#include "openMVG/matching/svg_matches.hpp"
#include "openMVG/image/image_drawing.hpp"

#include "third_party/stlplus3/filesystemSimplified/file_system.hpp"
#include "third_party/cmdLine/cmdLine.h"
#include <memory>
#include <string>
#include <regex>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fstream>
#include <iomanip>

using namespace openMVG;
using namespace openMVG::image;
using namespace std;

struct mPoint {
	double x;
	double y;
};

string get_standard_line(string tar) {
	if (tar == "ground_truth_xy.txt") {
		return "x";
	}
	else if (tar == "ground_truth_xz.txt") {
		return "z";
	}
	else {
		return "y";
	}
}

string get_compare_line(string tar) {
	if (tar == "ground_truth_xy.txt") {
		return "y";
	}
	else if (tar == "ground_truth_xz.txt") {
		return "x";
	}
	else {
		return "z";
	}
}

void read_point(const std::string file_root, vector<openMVG::features::PointFeature> &pointsF) {
	string x,y;
	ifstream inf(file_root);
	while (!inf.eof()) {
		inf >> x;
		if (inf.eof())break;
		inf >> y;
		openMVG::features::PointFeature p(std::atof(x.c_str()),std::atof(y.c_str()));
		pointsF.push_back(p);
		std::cout << x << "," << y << std::endl;
	}
	pointsF.pop_back();
}

int main(int argc, char **argv) {

	int step = 11;
	//step = 2 拼接rgb图
	//step = 3 拼接灰度图
	//step = 4 画灰度结果
	//step = 5 画rgb结果
	//step = 6 点云缩放. 
	//step = 7 Aln对齐;
	//step = 8 投影变化
	//step = 9 转换格式
	//step = 10 计算投影结果的bbox
	//step = 11 计算横向准确度
	//step = 12 对点云进行放大，便于进行对齐


	if (step == 2) {
		
		const string jpg_filenameL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img1.jpg";
		const string jpg_filenameR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img0.jpg";
		const string jpg_filenameM = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img2.jpg";
		const string pointlistL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/1.txt";
		const string pointlistR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/2.txt";
		const string pointlistM = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/3.txt";
		const int radiu = 5;
		const int M = 800;

		Image<RGBColor> imageL, imageR, imageM;
		//Image<unsigned char> imageL, imageR, imageM;
		ReadImage(jpg_filenameL.c_str(), &imageL);
		ReadImage(jpg_filenameR.c_str(), &imageR);
		ReadImage(jpg_filenameM.c_str(), &imageM);
		//assert(imageL.data() && imageR.data());

		// Call Keypoint Extractor
		using namespace openMVG::features;
		
		vector<PointFeature> pointsL,pointsR,pointsM;
		read_point(pointlistL, pointsL);
		read_point(pointlistR, pointsR);
		read_point(pointlistM, pointsM);
		int countL = 0, countR = 0, countM = 0;
		double cx = 0, cy = 0;

		vector<PointFeature>::const_iterator t;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			cx += abs(pointsL[countL].x() - pointsR[countL].x());
			cy += abs(pointsL[countL].y() - pointsR[countL].y());
			if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0) {
				//openMVG::image::DrawCircle(pointsL[countL].x(), pointsL[countL].y(), radiu, 255, &imageL);
				openMVG::image::DrawCircle(pointsL[countL].x(), pointsL[countL].y(), radiu, RGBColor(255, 255, 0), &imageL);
			}
			countL++;
		}

		for (t = pointsR.begin(); t != pointsR.end(); t++) {
			if (pointsR[countR].x() > 0 && pointsR[countR].y() > 0) {
				//openMVG::image::DrawCircle(pointsR[countR].x(), pointsR[countR].y(), radiu, 255, &imageR);
				openMVG::image::DrawCircle(pointsR[countR].x(), pointsR[countR].y(), radiu, RGBColor(255, 255, 0), &imageR);
			}
			countR++;
		}

		for (t = pointsM.begin(); t != pointsM.end(); t++) {
			if (pointsM[countM].x() > 0 && pointsM[countM].y() > 0) {
				//openMVG::image::DrawCircle(pointsM[countM].x(), pointsM[countM].y(), radiu, 255, &imageM);
				openMVG::image::DrawCircle(pointsM[countM].x(), pointsM[countM].y(), radiu, RGBColor(255, 255, 0), &imageM);
			}
			countM++;
		}

		double concentx = cx / countL, concenty = cy / countL;

		//- Show images side by side
		//Image<unsigned char> concat;
		Image<RGBColor> concat;
		ConcatH(imageL, imageR, concat);
		const string out_filename = "00_images.jpg";
		WriteImage(out_filename.c_str(), concat);

		countL = 0;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			if (abs(abs(pointsL[countL].x() - pointsR[countL].x()) - concentx) < 100 && abs(abs(pointsL[countL].y() - pointsR[countL].y()) - concenty) < 100) {
				if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0 && pointsR[countL].x() > 0 && pointsR[countL].y() > 0) {
					//openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(),pointsR[countL].x() + M, pointsR[countL].y() , 255, &concat);
					openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(), pointsR[countL].x() + M, pointsR[countL].y(), RGBColor(255,255,0), &concat);
				}
			}
			countL++;
		}

		const string out_filename2 = "01_images.jpg";
		WriteImage(out_filename2.c_str(), concat);

		//Image<unsigned char> concat2;
		Image<RGBColor> concat2;
		ConcatH(imageL, imageM, concat2);
		const string out_filename3 = "02_images.jpg";
		WriteImage(out_filename3.c_str(), concat2);

		countL = 0;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			if (abs(abs(pointsL[countL].x() - pointsR[countL].x()) - concentx) < 100 && abs(abs(pointsL[countL].y() - pointsR[countL].y()) - concenty) < 100) {
				if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0 && pointsM[countL].x() > 0 && pointsM[countL].y() > 0) {
					//openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(), pointsM[countL].x() + M, pointsM[countL].y(), 255, &concat2);
					openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(), pointsM[countL].x() + M, pointsM[countL].y(), RGBColor(255,255,0), &concat2);
				}
			}
			countL++;
		}

		//getchar();

		const string out_filename4 = "03_images.jpg";
		WriteImage(out_filename4.c_str(), concat2);

		//{
		//	Features2SVG
		//	(
		//		jpg_filenameL,
		//		{ imageL.Width(), imageL.Height() },
		//		regions_perImage.at(0)->GetRegionsPositions(),
		//		jpg_filenameR,
		//		{ imageR.Width(), imageR.Height() },
		//		regions_perImage.at(1)->GetRegionsPositions(),
		//		"01_features.svg"
		//	);
		//}


		//matching::IndMatches vec_PutativeMatches;
		//matching::DistanceRatioMatch(
		//	0.8, matching::BRUTE_FORCE_L2,
		//	*regions_perImage.at(0).get(),
		//	*regions_perImage.at(1).get(),
		//	vec_PutativeMatches);

		//{
		//	const bool bVertical = true;
		//	Matches2SVG
		//	(
		//		jpg_filenameL,
		//		{ imageL.Width(), imageL.Height() },
		//		regions_perImage.at(0)->GetRegionsPositions(),
		//		jpg_filenameR,
		//		{ imageR.Width(), imageR.Height() },
		//		regions_perImage.at(1)->GetRegionsPositions(),
		//		vec_PutativeMatches,
		//		"02_Matches.svg",
		//		bVertical
		//	);
		//}

		//// Display some statistics
		//std::cout
		//	<< regions_perImage.at(0)->RegionCount() << " #Features on image A" << std::endl
		//	<< regions_perImage.at(1)->RegionCount() << " #Features on image B" << std::endl
		//	<< vec_PutativeMatches.size() << " #matches with Distance Ratio filter" << std::endl;
	
	}
 
	if (step == 3) {

		const string jpg_filenameL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img1.jpg";
		const string jpg_filenameR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img0.jpg";
		const string jpg_filenameM = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img2.jpg";
		const string pointlistL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/1.txt";
		const string pointlistR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/2.txt";
		const string pointlistM = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/3.txt";
		const int radiu = 5;
		const int M = 850;

		Image<unsigned char> imageL, imageR, imageM;
		ReadImage(jpg_filenameL.c_str(), &imageL);
		ReadImage(jpg_filenameR.c_str(), &imageR);
		ReadImage(jpg_filenameM.c_str(), &imageM);
		//assert(imageL.data() && imageR.data());

		// Call Keypoint Extractor
		using namespace openMVG::features;

		vector<PointFeature> pointsL, pointsR, pointsM;
		read_point(pointlistL, pointsL);
		read_point(pointlistR, pointsR);
		read_point(pointlistM, pointsM);
		int countL = 0, countR = 0, countM = 0;
		double cx = 0, cy = 0;

		vector<PointFeature>::const_iterator t;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			cx += abs(pointsL[countL].x() - pointsR[countL].x());
			cy += abs(pointsL[countL].y() - pointsR[countL].y());
			if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0) {
				openMVG::image::DrawCircle(pointsL[countL].x(), pointsL[countL].y(), radiu, 255, &imageL);
			}
			countL++;
		}

		for (t = pointsR.begin(); t != pointsR.end(); t++) {
			if (pointsR[countR].x() > 0 && pointsR[countR].y() > 0) {
				openMVG::image::DrawCircle(pointsR[countR].x(), pointsR[countR].y(), radiu, 255, &imageR);
			}
			countR++;
		}

		for (t = pointsM.begin(); t != pointsM.end(); t++) {
			if (pointsM[countM].x() > 0 && pointsM[countM].y() > 0) {
				openMVG::image::DrawCircle(pointsM[countM].x(), pointsM[countM].y(), radiu, 255, &imageM);
			}
			countM++;
		}

		double concentx = cx / countL, concenty = cy / countL;

		//- Show images side by side
		Image<unsigned char> concat;
		ConcatH(imageL, imageR, concat);
		const string out_filename = "00_images.jpg";
		WriteImage(out_filename.c_str(), concat);

		countL = 0;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			if (abs(abs(pointsL[countL].x() - pointsR[countL].x()) - concentx) < 100 && abs(abs(pointsL[countL].y() - pointsR[countL].y()) - concenty) < 100) {
				if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0 && pointsR[countL].x() > 0 && pointsR[countL].y() > 0) {
					openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(),pointsR[countL].x() + M, pointsR[countL].y() , 255, &concat);
				}
			}
			countL++;
		}

		const string out_filename2 = "01_images.jpg";
		WriteImage(out_filename2.c_str(), concat);

		Image<unsigned char> concat2;
		ConcatH(imageL, imageM, concat2);
		const string out_filename3 = "02_images.jpg";
		WriteImage(out_filename3.c_str(), concat2);

		countL = 0;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			if (abs(abs(pointsL[countL].x() - pointsR[countL].x()) - concentx) < 100 && abs(abs(pointsL[countL].y() - pointsR[countL].y()) - concenty) < 100) {
				if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0 && pointsM[countL].x() > 0 && pointsM[countL].y() > 0) {
					openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(), pointsM[countL].x() + M, pointsM[countL].y(), 255, &concat2);
				}
			}
			countL++;
		}
		const string out_filename4 = "03_images.jpg";
		WriteImage(out_filename4.c_str(), concat2);
	}

	if (step == 4) {

		const string jpg_filenameL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img1.jpg";
		const string jpg_filenameR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img2.jpg";
		const string pointlistL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/1.txt";
		const string pointlistR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/2.txt";
		const int radiu = 5;
		const int M = 850;

		Image<unsigned char> imageL, imageR;
		ReadImage(jpg_filenameL.c_str(), &imageL);
		ReadImage(jpg_filenameR.c_str(), &imageR);
		//assert(imageL.data() && imageR.data());

		// Call Keypoint Extractor
		using namespace openMVG::features;

		vector<PointFeature> pointsL, pointsR;
		read_point(pointlistL, pointsL);
		read_point(pointlistR, pointsR);
		int countL = 0, countR = 0;
		double cx = 0, cy = 0;

		vector<PointFeature>::const_iterator t;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			cx += abs(pointsL[countL].x() - pointsR[countL].x());
			cy += abs(pointsL[countL].y() - pointsR[countL].y());
			if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0) {
				openMVG::image::DrawCircle(pointsL[countL].x(), pointsL[countL].y(), radiu, 255, &imageL);
			}
			countL++;
		}

		for (t = pointsR.begin(); t != pointsR.end(); t++) {
			if (pointsR[countR].x() > 0 && pointsR[countR].y() > 0) {
				openMVG::image::DrawCircle(pointsR[countR].x(), pointsR[countR].y(), radiu, 255, &imageR);
			}
			countR++;
		}

		double concentx = cx / countL, concenty = cy / countL;

		//- Show images side by side
		Image<unsigned char> concat;
		ConcatH(imageL, imageR, concat);
		const string out_filename = "00_images.jpg";
		WriteImage(out_filename.c_str(), concat);

		countL = 0;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			if (abs(abs(pointsL[countL].x() - pointsR[countL].x()) - concentx) < 1000 && abs(abs(pointsL[countL].y() - pointsR[countL].y()) - concenty) < 1000) {
				if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0 && pointsR[countL].x() > 0 && pointsR[countL].y() > 0) {
					openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(), pointsR[countL].x() + M, pointsR[countL].y(), 255, &concat);
				}
			}
			countL++;
		}

		const string out_filename2 = "01_images.jpg";
		WriteImage(out_filename2.c_str(), concat);

	}

	if (step == 5) {

		const string jpg_filenameL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img1.jpg";
		const string jpg_filenameR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/img3.jpg";
		const string pointlistL = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/1.txt";
		const string pointlistR = "C:/Users/FBJ/Desktop/vgg_oxford_feat_eval/step4/2.txt";
		const int radiu = 5;
		const int M = 800;

		Image<RGBColor> imageL, imageR;
		ReadImage(jpg_filenameL.c_str(), &imageL);
		ReadImage(jpg_filenameR.c_str(), &imageR);
		//assert(imageL.data() && imageR.data());

		// Call Keypoint Extractor
		using namespace openMVG::features;

		vector<PointFeature> pointsL, pointsR;
		read_point(pointlistL, pointsL);
		read_point(pointlistR, pointsR);
		int countL = 0, countR = 0;
		double cx = 0, cy = 0;

		vector<PointFeature>::const_iterator t;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			cx += abs(pointsL[countL].x() - pointsR[countL].x());
			cy += abs(pointsL[countL].y() - pointsR[countL].y());
			if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0) {
				openMVG::image::DrawCircle(pointsL[countL].x(), pointsL[countL].y(), radiu, RGBColor(255, 255, 0), &imageL);
			}
			countL++;
		}

		for (t = pointsR.begin(); t != pointsR.end(); t++) {
			if (pointsR[countR].x() > 0 && pointsR[countR].y() > 0) {
				openMVG::image::DrawCircle(pointsR[countR].x(), pointsR[countR].y(), radiu, RGBColor(255, 255, 0), &imageR);
			}
			countR++;
		}

		double concentx = cx / countL, concenty = cy / countL;

		//- Show images side by side
		Image<RGBColor> concat;
		ConcatH(imageL, imageR, concat);
		const string out_filename = "00_images.jpg";
		WriteImage(out_filename.c_str(), concat);

		countL = 0;
		for (t = pointsL.begin(); t != pointsL.end(); t++) {
			if (abs(abs(pointsL[countL].x() - pointsR[countL].x()) - concentx) < 1000 && abs(abs(pointsL[countL].y() - pointsR[countL].y()) - concenty) < 1000) {
				if (pointsL[countL].x() > 0 && pointsL[countL].y() > 0 && pointsR[countL].x() > 0 && pointsR[countL].y() > 0) {
					openMVG::image::DrawLine(pointsL[countL].x(), pointsL[countL].y(), pointsR[countL].x() + M, pointsR[countL].y(), RGBColor(255, 255, 0), &concat);
				}
			}
			countL++;
		}

		const string out_filename2 = "01_images.jpg";
		WriteImage(out_filename2.c_str(), concat);

	}

	if (step == 12) {
		fstream point("C:/Users/FBJ/Desktop/data/Sofa07/SIFT_ANATOMY/1b.xyz");
		ofstream result("C:/Users/FBJ/Desktop/data/Sofa07/SIFT_ANATOMY/1c.xyz");
		//AKAZE_FLOAT
		//AKAZE_MLDB
		//SIFT
		//SIFT_ANATOMY
		//VisualSFM

		string temp;
		double zone_rate = 1;
		double x, y, z, l1, l2, l3;
		if (!point.is_open()) {
			cout << "Can not open file." << endl;
		}
		cin.sync();
		cin.rdbuf(point.rdbuf());
		while (getline(point, temp)) {
			cin >> x >> y >> z;
			cin >> l1 >> l2 >> l3;
			x *= zone_rate;
			y *= zone_rate;
			z *= zone_rate;
			result << x+2 << " " << y << " " << z << " ";
			result << l1 << " " << l2 << " " << l3 << endl;
		}
		result.close();
	}

	if (step == 6) {

		fstream point_gt ("C:/Users/FBJ/Desktop/data/Desk05/AKAZE_FLOAT/input.txt");
		fstream point_re ("C:/Users/FBJ/Desktop/data/Desk05/AKAZE_FLOAT/step6.txt");
		//AKAZE_FLOAT
		//AKAZE_MLDB
		//SIFT
		//SIFT_ANATOMY
		//VisualSFM
		vector<double[6]> log_gt,log_re;
		double para_insert[6];
		string temp;
		int max_size1 = 0, max_size2 = 0;
		double re_mx, re_my, re_mz, re_x, re_y, re_z;
		double gt_mx, gt_my, gt_mz, gt_x, gt_y, gt_z;
		double ax, ay, az, bx, by, bz;
		double tax, tay, taz, tbx, tby, tbz;
		int b = 0;

		if (!point_gt.is_open()) {
			cout << "Can not open file." << endl;
		}
		cin.rdbuf(point_gt.rdbuf());
		while (getline(point_gt,temp)) {
			b = 0;
			if (max_size1 == 0) {
				cin >> gt_mx >> gt_my >> gt_mz; 
				cin >> para_insert[3] >> para_insert[4] >> para_insert[5];
				gt_x = gt_mx; gt_y = gt_my; gt_z = gt_mz;
				tax = gt_mx; tay = gt_my; taz = gt_mz;
			}
			else {
				for (int i = 0; i < 6; i++) {
					cin >> para_insert[i];
					//cout << para_insert[i];
					if (i == 0) {
						if (gt_mx < para_insert[i])b = 1;
						gt_mx = gt_mx < para_insert[i] ? para_insert[i] : gt_mx;
						gt_x = gt_x > para_insert[i] ? para_insert[i] : gt_x;
						tax += para_insert[i];
					}
					else if (i == 1) {
						gt_my = gt_my < para_insert[i] ? para_insert[i] : gt_my;
						gt_y = gt_y > para_insert[i] ? para_insert[i] : gt_y;
						tay += abs(para_insert[i]);
					}
					else if (i == 2) {
						gt_mz = gt_mz < para_insert[i] ? para_insert[i] : gt_mz;
						gt_z = gt_z > para_insert[i] ? para_insert[i] : gt_z;
						taz += para_insert[i];
					}
				}
				if (b == 1) {
					ax = para_insert[0];
					ay = para_insert[1];
					az = para_insert[2];
				}
			}
			max_size1++;
			//log_gt.push_back(para_insert);
			//cout << std::endl;
		}

		gt_x = gt_mx - gt_x; 
		gt_y = gt_my - gt_y; 
		gt_z = gt_mz - gt_z;

		if (!point_re.is_open()) {
			cout << "Can not open file." << endl;
		}
		cin.rdbuf(point_re.rdbuf());
		while (getline(point_re, temp)) {
			b = 0;
			if (max_size2 == 0) {
				cin >> re_mx >> re_my >> re_mz;
				cin >> para_insert[3] >> para_insert[4] >> para_insert[5];
				re_x = re_mx; re_y = re_my; re_z = re_mz;
				tbx = re_mx; tby = re_my; tbz = re_mz;
			}
			else {
				for (int i = 0; i < 6; i++) {
					cin >> para_insert[i];
					//cout << para_insert[i];
					if (i == 0) {
						if (re_mx < para_insert[i])b = 1;
						re_mx = re_mx < para_insert[i] ? para_insert[i] : re_mx;
						re_x = re_x > para_insert[i] ? para_insert[i] : re_x;
						tbx += para_insert[i];
					}
					else if (i == 1) {
						re_my = re_my < para_insert[i] ? para_insert[i] : re_my;
						re_y = re_y > para_insert[i] ? para_insert[i] : re_y;
						tby += abs(para_insert[i]);
					}
					else if (i == 2) {
						re_mz = re_mz < para_insert[i] ? para_insert[i] : re_mz;
						re_z = re_z > para_insert[i] ? para_insert[i] : re_z;
						tbz += para_insert[i];
					}
				}
				if (b == 1) {
					bx = para_insert[0];
					by = para_insert[1];
					bz = para_insert[2];
				}
			}
			max_size2++;
			//log_gt.push_back(para_insert);
			//cout << std::endl;
		}

		re_x = re_mx - re_x;
		re_y = re_my - re_y; 
		re_z = re_mz - re_z;
		double x_rate = gt_x / re_x, y_rate = gt_y / re_y, z_rate = gt_z / re_z;
		double nx, ny, nz;
		nx = bx*x_rate - ax;
		ny = by*y_rate - ay;
		nz = bz*z_rate - az;

		//calculate the average axis point
		tax = tax / max_size1;
		tay = tay / max_size1;
		taz = taz / max_size1;
		tbx = tbx / max_size2;
		tby = tby / max_size2;
		tbz = tbz / max_size2;
		cout << tax << ";" << tay << ";" << taz << endl;
		cout << tbx << ";" << tby << ";" << tbz << endl;


		//output aln resize points.
		fstream point_re2("C:/Users/FBJ/Desktop/point/1a.txt");
		ofstream ofout("C:/Users/FBJ/Desktop/point/re5.txt");
		cin.rdbuf(point_re2.rdbuf());
		while (getline(point_re2, temp)) {
			for (int i = 0; i < 6; i++) {
				cin >> para_insert[i];
				//if(i == 0)ofout << para_insert[i]*x_rate << " ";
				//else if (i == 1)ofout << para_insert[i] * y_rate << " ";
				//else if (i == 2)ofout << para_insert[i] * z_rate << " ";

				if (i == 0)ofout << (para_insert[i] + (tax - tbx))*x_rate << " ";
				else if (i == 1)ofout << (para_insert[i] + 0.4 + (tay - tby)) * y_rate << " ";
				else if (i == 2)ofout << (para_insert[i] + (taz - tbz)) * z_rate << " ";

				//if (i == 0)ofout << (para_insert[i] + (gt_mx - re_mx))*x_rate << " ";
				//else if (i == 1)ofout << (para_insert[i] + (gt_my - re_my)) * y_rate << " ";
				//else if (i == 2)ofout << (para_insert[i] + (gt_mz - re_mz)) * z_rate << " ";

				//if (i == 0)ofout << para_insert[i] * x_rate - nx << " ";
				//else if (i == 1)ofout << para_insert[i] * y_rate - ny << " ";
				//else if (i == 2)ofout << para_insert[i] * z_rate - nz << " ";
				else if (i == 5)ofout << para_insert[i];
				else ofout << para_insert[i] << " ";


				//ofout << para_insert[i] + 5 << " ";
			}
			ofout << endl;
		}
		ofout.close();
		getchar();
	}

	if (step == 7) {
		//fstream aln("C:/Users/FBJ/Desktop/data/Sofa06/SIFT/f.txt");
		//fstream re("C:/Users/FBJ/Desktop/data/Sofa06/SIFT/input.txt");
		//ofstream re2("C:/Users/FBJ/Desktop/data/Sofa06/SIFT/step7.txt");
		fstream aln("C:/Users/FBJ/Desktop/data/Desk05/VisualSFM/f.txt");
		fstream re("C:/Users/FBJ/Desktop/data/Desk05/VisualSFM/input.txt");
		ofstream re2("C:/Users/FBJ/Desktop/data/Desk05/VisualSFM/step7.txt");
		//AKAZE_FLOAT        //Desk05
		//AKAZE_MLDB         //Sofa05  
		//SIFT               //Sofa06
		//SIFT_ANATOMY       
		//VisualSFM
		vector<double[6]> input;
		double para_insert[6];
		double aln_1a[3][4];
		double d;
		string temp;
		string is;

		if (!aln.is_open()) {
			cout << "Can not open file!" << std::endl;
		}
		cin.rdbuf(aln.rdbuf());
		//cin >> is >> is;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				cin >> d;
				aln_1a[i][j] = d;
				cout << d << " ";
			}
			cout << endl;
		}

		if (!re.is_open()) {
			cout << "Can not open file!" << endl;
		}
		double x, y, z;
		double k1, k2, k3;
		cin.sync();
		cin.rdbuf(re.rdbuf());
		while (getline(re, temp)) {
			cin >> x >> y >> z;
			cin >> k1 >> k2 >> k3;
			//cout << x << y << z << endl;
			re2 << x*aln_1a[0][0] + y*aln_1a[0][1] + z*aln_1a[0][2] + aln_1a[0][3] << " ";
			re2 << x*aln_1a[1][0] + y*aln_1a[1][1] + z*aln_1a[1][2] + aln_1a[1][3] << " ";
			re2 << x*aln_1a[2][0] + y*aln_1a[2][1] + z*aln_1a[2][2] + aln_1a[2][3] << " ";
			re2 << k1 << " " << k2 << " " << k3 << endl;
			//getchar();
		}
		re2.close();
		//getchar();
	}

	if (step == 8) {//将结果投影到平面上
		string root1[] = { "Sofa06" };
		string root2[] = { "AKAZE_MLDB", "SIFT", "SIFT_ANATOMY", "VisualSFM" };
		for (int i = 0; i < (sizeof(root1) / sizeof(root1[0])); i++) {
			for (int j = 0; j < (sizeof(root2) / sizeof(root2[0])); j++) {
				fstream re("C:/Users/FBJ/Desktop/data/"+root1[i]+"/"+root2[j]+"/Sofa06.txt");
				ofstream re2("C:/Users/FBJ/Desktop/data/" + root1[i] + "/" + root2[j] + "/flat/ground_truth_xy.txt");
				ofstream re3("C:/Users/FBJ/Desktop/data/" + root1[i] + "/" + root2[j] + "/flat/ground_truth_xz.txt");
				ofstream re4("C:/Users/FBJ/Desktop/data/" + root1[i] + "/" + root2[j] + "/flat/ground_truth_yz.txt");
				//fstream re("C:/Users/FBJ/Desktop/data/Desk05/AKAZE_FLOAT/Desk05.txt");
				//ofstream re2("C:/Users/FBJ/Desktop/data/Desk05/AKAZE_FLOAT/flat/ground_truth_xy.txt");
				//ofstream re3("C:/Users/FBJ/Desktop/data/Desk05/AKAZE_FLOAT/flat/ground_truth_xz.txt");
				//ofstream re4("C:/Users/FBJ/Desktop/data/Desk05/AKAZE_FLOAT/flat/ground_truth_yz.txt");
				cout << "C:/Users/FBJ/Desktop/data/" + root1[i] + "/" + root2[j] + "/flat/result_yz.txt" << endl;
					//AKAZE_FLOAT        //Desk05
					//AKAZE_MLDB         //Sofa05  
					//SIFT               //Sofa06
					//SIFT_ANATOMY       
					//VisualSFM
				vector<double[6]> input;
				double para_insert[6];
				double aln_1a[3][4];
				double d;
				string temp;
				string is;

				if (!re.is_open()) {
					cout << "Can not open file!" << endl;
				}
				double x, y, z;
				double k1, k2, k3;
				cin.sync();
				cin.rdbuf(re.rdbuf());
				while (getline(re, temp)) {
					cin >> x >> y >> z;
					cin >> k1 >> k2 >> k3;
					//cout << x << y << z << endl;
					re2 << x << " ";
					re2 << y << " ";
					re2 << 0 << " ";
					re2 << k1 << " " << k2 << " " << k3 << endl;
					re3 << x << " ";
					re3 << 0 << " ";
					re3 << z << " ";
					re3 << k1 << " " << k2 << " " << k3 << endl;
					re4 << 0 << " ";
					re4 << y << " ";
					re4 << z << " ";
					re4 << k1 << " " << k2 << " " << k3 << endl;
					//getchar();
				}
				re.close();
				re2.close();
				re3.close();
				re4.close();
			}
		}
	}

	if (step == 9) {//车辆识别中对标注的格式进行转换
		const string point_pos = "C:/Users/FBJ/Desktop/Traffic_Sign_detection-master/r2.txt";
		ofstream result("C:/Users/FBJ/Desktop/Traffic_Sign_detection-master/result_new2.txt");
		string x, y;
		int p, count = 0, line = 1;
		char c;
		ifstream inf(point_pos);
		while (!inf.eof()) {
			inf >> x;
			//std:; cout << x << endl;
			for (int i = 0; i < x.size() - 1; i++) {
				if (x[i] == ',' || x[i] == ':') {
					//if(x[i] == ';' && x[i+1] != 'i')
					p = 0;
					i++;
					//while (x[i] != ';') {
					while (x[i] != ',') {
						c = x[i];
						p = p * 10 + (c - '0');
						i++;
					}
					i--;
					count++;
					if (count == 1) {
						result << line << " ";
						line++;
						result << p << " ";
					}
					else if (count == 4) {
						count = 0;
						result << p << endl;
					}
					else {
						result << p << " ";
					}
				}
			}
		}
		result.close();


		return 0;
	}

	if (step == 10) {//计算投影结果的bbox
		string base_root = "C:/Users/FBJ/Desktop/data/";
		string target_root[] = { "ground_truth_xy.txt","ground_truth_xz.txt", "ground_truth_yz.txt", "result_xy.txt", "result_xz.txt", "result_yz.txt" };
		string root1[] = { "Sofa06" };
		string root2[] = { "AKAZE_MLDB", "SIFT", "SIFT_ANATOMY", "VisualSFM" };
		for (int i = 0; i < (sizeof(root1) / sizeof(root1[0])); i++) {
			for (int j = 0; j < (sizeof(root2) / sizeof(root2[0])); j++) {
				int size = sizeof(target_root) / sizeof(target_root[0]);
				ofstream re2("C:/Users/FBJ/Desktop/data/" + root1[i] + "/" + root2[j] + "/flat/result_bbox.txt");
				while (size > 0) {
					size -= 1;
					//针对读取方式，在首行空出一行，尾行不换行
					re2 << endl;
					fstream re(base_root + root1[i] + "/" + root2[j] + "/flat/" + target_root[size]);
					cout << base_root + root1[i] + "/" + root2[j] + "/" + target_root[size] << endl;
					vector<double[6]> input;
					double para_insert[6];
					double aln_1a[3][4];
					double d;
					string temp;
					string is;

					if (!re.is_open()) {
						cout << "Can not open file!" << endl;
					}
					double x, y, z;
					double k1, k2, k3;
					double max_lx, max_rx, max_ly, max_ry, max_lz, max_rz;
					cin.sync();
					cin.rdbuf(re.rdbuf());
					int i = 0;
					while (getline(re, temp)) {
						cin >> x >> y >> z;
						cin >> k1 >> k2 >> k3;
						if (i++ == 0) {
							max_lx = max_rx = x;
							max_ly = max_ry = y;
							max_lz = max_rz = z;
						}
						else {
							if (x < max_lx)max_lx = x;
							if (x > max_rx)max_rx = x;
							if (y < max_ly)max_ly = y;
							if (y > max_ry)max_ry = y;
							if (z < max_lz)max_lz = z;
							if (z > max_rz)max_rz = z;
						}
					}
					re2 << target_root[size] << " ";
					re2 << max_lx << " " << max_rx << " ";
					re2 << max_ly << " " << max_ry << " ";
					re2 << max_lz << " " << max_rz;
				}
				re2.close();
			}
		}
	}

	if (step == 11) {//计算横向准确度
		string add[] = { "xy.txt","xz.txt","yz.txt" };
		string root1[] = { "Sofa05","Sofa06" };
		string root2[] = { "AKAZE_FLOAT","AKAZE_MLDB", "SIFT", "SIFT_ANATOMY", "VisualSFM" };
		for (int ii = 0; ii < (sizeof(root1) / sizeof(root1[0])); ii++) {
			for (int ij = 0; ij < (sizeof(root2) / sizeof(root2[0])); ij++) {
				for (int il = 0; il < (sizeof(add) / sizeof(add[0])); il++) {
					string base_result_root = "C:/Users/FBJ/Desktop/data/" + root1[ii] + "/" + root2[ij] + "/flat/result1_";
					string base_result_root1 = "C:/Users/FBJ/Desktop/data/" + root1[ii] + "/" + root2[ij] + "/flat/result1_type1_";
					string base_result_root2 = "C:/Users/FBJ/Desktop/data/" + root1[ii] + "/" + root2[ij] + "/flat/result1_type2_";
					string base_result_root3 = "C:/Users/FBJ/Desktop/data/" + root1[ii] + "/" + root2[ij] + "/flat/result1_type3_";
					string base_root = "C:/Users/FBJ/Desktop/data/" + root1[ii] + "/" + root2[ij] + "/flat/";
					string target_root = "ground_truth_";
					string target_root2 = "result_";
					target_root += add[il]; target_root2 += add[il];

					string standard_line, compare_line;
					standard_line = get_standard_line(target_root);
					compare_line = get_compare_line(target_root);
					string bbox_root = "C:/Users/FBJ/Desktop/data/" + root1[ii] + "/" + root2[ij] + "/flat/result_bbox.txt";

					//获取bbox的位置
					double max_lx_gt, max_rx_gt, max_ly_gt, max_ry_gt, max_lz_gt, max_rz_gt;
					double max_lx, max_rx, max_ly, max_ry, max_lz, max_rz;
					double ig1, ig2, ig3, ig4, ig5, ig6;
					fstream bbox(bbox_root);
					if (!bbox.is_open()) {
						cout << "read config error" << endl;
					}
					string bbox_type;
					string temp;
					cin.sync();
					cin.rdbuf(bbox.rdbuf());
					while (getline(bbox, temp)) {
						cin >> bbox_type;
						if (bbox_type == target_root) {
							cin >> max_lx_gt >> max_rx_gt >> max_ly_gt >> max_ry_gt >> max_lz_gt >> max_rz_gt;
						}
						else if (bbox_type == target_root2) {
							cin >> max_lx >> max_rx >> max_ly >> max_ry >> max_lz >> max_rz;
						}
						else {
							cin >> ig1 >> ig2 >> ig3 >> ig4 >> ig5 >> ig6;
						}
						cout << bbox_type << endl;
					}
					//根据bbox计算standardline和compareline上的偏移量
					double standard_move, compare_move;
					if (standard_line == "x") {
						standard_move = (max_lx_gt + max_rx_gt - max_lx - max_rx) / 2;
						compare_move = (max_ly_gt + max_ry_gt - max_ly - max_ry) / 2;
					}
					else if (standard_line == "y") {
						standard_move = (max_ly_gt + max_ry_gt - max_ly - max_ry) / 2;
						compare_move = (max_lz_gt + max_rz_gt - max_lz - max_rz) / 2;
					}
					else {
						standard_move = (max_lz_gt + max_rz_gt - max_lz - max_rz) / 2;
						compare_move = (max_lx_gt + max_rx_gt - max_lx - max_rx) / 2;
					}
					//计算bbox在非投影轴的高
					double distance;
					if (standard_line == "x") {
						distance = abs(max_ly_gt - max_ry_gt);
					}
					else if (standard_line == "y") {
						distance = abs(max_lz_gt - max_rz_gt);
					}
					else {
						distance = abs(max_lx_gt - max_rx_gt);
					}
					//找到标准轴的开始和结尾
					fstream gt(base_root + target_root);
					double standard_start, standard_end;
					double use;
					double x, y, z, r, g, b;
					if (!gt.is_open()) {
						cout << "Can not open file!" << endl;
					}
					cin.sync();
					cin.rdbuf(gt.rdbuf());
					int i = 0;
					while (getline(gt, temp)) {
						cin >> x >> y >> z;
						cin >> r >> g >> b;
						if (standard_line == "x")use = x;
						else if (standard_line == "y")use = y;
						else { use = z; }
						if (i++ == 0) {
							standard_start = standard_end = use;
						}
						else {
							if (standard_start > use)standard_start = use;
							if (standard_end < use)standard_end = use;
						}
					}
					gt.close();
					//以threshold为单位，对groundtruth的上下边界进行记录
					//读取所有点，储存在vector结构中，所有坐标轴组合统一为x,y
					mPoint m_point;
					vector<mPoint> all_point_gt, all_point_re;
					fstream gt2(base_root + target_root);
					fstream re(base_root + target_root2);
					if (!re.is_open()) {
						cout << "Can not open file!" << endl;
					}
					cin.sync();
					cin.rdbuf(gt2.rdbuf());
					while (getline(gt2, temp)) {
						cin >> x >> y >> z;
						cin >> r >> g >> b;
						if (standard_line == "x") {
							m_point.x = x;
							m_point.y = y;
						}
						else if (standard_line == "y") {
							m_point.x = y;
							m_point.y = z;
						}
						else {
							m_point.x = z;
							m_point.y = x;
						}
						all_point_gt.push_back(m_point);
					}
					gt2.close();
					cin.sync();
					cin.rdbuf(re.rdbuf());
					while (getline(re, temp)) {
						cin >> x >> y >> z;
						cin >> r >> g >> b;
						if (standard_line == "x") {
							m_point.x = x;
							m_point.y = y;
						}
						else if (standard_line == "y") {
							m_point.x = y;
							m_point.y = z;
						}
						else {
							m_point.x = z;
							m_point.y = x;
						}
						all_point_re.push_back(m_point);
					}
					gt.close();
					//按照间隔进行上下界寻找,统计竖向点分布
					int lenth = 15;
					double threshold = (standard_end - standard_start) / lenth;
					cout << compare_move << endl;
					vector<double> gt_up_log, gt_down_log, re_up_log, re_down_log;
					vector<double> gt_point, re_point;
					double now_start = standard_start, now_end = standard_start + threshold;
					double now_up_gt, now_down_gt, now_up_re, now_down_re;
					//用于修正结果的范围
					double fix_start, fix_end, fix_threshold = threshold;
					int if_fix = 0, fix_count = 0;
					//用于统计每段点占点云的总比例
					int now_point = 0, total_point_gt = all_point_gt.size(), total_point_re = all_point_re.size();
					for (i = 0; i < lenth; i++) {
						if (if_fix == 0) {
							int k = 0;
							for (int j = 0; j < all_point_gt.size(); j++) {
								if (all_point_gt[j].x >= now_start && all_point_gt[j].x <= now_end) {
									now_point++;
									if (k++ == 0) {
										now_up_gt = now_down_gt = all_point_gt[j].y;
									}
									else {
										if (now_up_gt < all_point_gt[j].y)now_up_gt = all_point_gt[j].y;
										if (now_down_gt > all_point_gt[j].y)now_down_gt = all_point_gt[j].y;
									}
								}
							}
							gt_point.push_back(double(now_point) / total_point_gt);
							k = 0; now_point = 0;
							for (int j = 0; j < all_point_re.size(); j++) {
								if (all_point_re[j].x >= (now_start + standard_move) && all_point_re[j].x <= (now_end + standard_move)) {
									now_point++;
									if (k++ == 0) {
										now_up_re = now_down_re = all_point_re[j].y + compare_move;
									}
									else {
										if (now_up_re < (all_point_re[j].y + compare_move))now_up_re = all_point_re[j].y + compare_move;
										if (now_down_re >(all_point_re[j].y + compare_move))now_down_re = all_point_re[j].y + compare_move;
									}
								}
							}
							re_point.push_back(double(now_point) / total_point_re);
							k = 0; now_point = 0;
							if (abs(now_up_gt - now_up_re) < fix_threshold && abs(now_down_gt - now_down_re) < fix_threshold) {
								gt_up_log.push_back(now_up_gt);
								gt_down_log.push_back(now_down_gt);
								re_up_log.push_back(now_up_re);
								re_down_log.push_back(now_down_re);
								now_start += threshold;
								now_end += threshold;
								if_fix = 0;
								fix_count = 0;
							}
							else {
								fix_start = now_start;
								fix_end = now_end;
								if_fix = 1;
								fix_count = 1;
								i--;
							}
						}
						else if (if_fix == 1) {
							int k = 0;
							fix_end += (threshold / 2);
							fix_start -= (threshold / 2);
							for (int j = 0; j < all_point_gt.size(); j++) {
								if (all_point_gt[j].x >= fix_start && all_point_gt[j].x <= fix_end) {
									if (k++ == 0) {
										now_up_gt = now_down_gt = all_point_gt[j].y;
									}
									else {
										if (now_up_gt < all_point_gt[j].y)now_up_gt = all_point_gt[j].y;
										if (now_down_gt > all_point_gt[j].y)now_down_gt = all_point_gt[j].y;
									}
								}
							}
							k = 0;
							for (int j = 0; j < all_point_re.size(); j++) {
								if (all_point_re[j].x >= (fix_start + standard_move) && all_point_re[j].x <= (fix_end + standard_move)) {
									if (k++ == 0) {
										now_up_re = now_down_re = all_point_re[j].y + compare_move;
									}
									else {
										if (now_up_re < (all_point_re[j].y + compare_move))now_up_re = all_point_re[j].y + compare_move;
										if (now_down_re >(all_point_re[j].y + compare_move))now_down_re = all_point_re[j].y + compare_move;
									}
								}
							}
							k = 0;
							if (abs(now_up_gt - now_up_re) < fix_threshold && abs(now_down_gt - now_down_re) < fix_threshold) {
								gt_up_log.push_back(now_up_gt);
								gt_down_log.push_back(now_down_gt);
								re_up_log.push_back(now_up_re);
								re_down_log.push_back(now_down_re);
								now_start += threshold;
								now_end += threshold;
								if_fix = 0;
								fix_count = 0;
							}
							else if (fix_count >= 2) {
								gt_up_log.push_back(now_up_gt);
								gt_down_log.push_back(now_down_gt);
								re_up_log.push_back(now_up_re);
								re_down_log.push_back(now_down_re);
								now_start += threshold;
								now_end += threshold;
								if_fix = 0;
								fix_count = 0;
							}
							else {
								fix_start -= (threshold / 2);
								fix_end += (threshold / 2);
								if_fix = 1;
								fix_count++;
								i--;
							}
						}
						//计算竖向跨度
						vector<double> cross_gt, cross_re;
						double temp_cross;
						for (int j = 0; j < gt_up_log.size(); j++) {
							temp_cross = abs(gt_up_log[j] - gt_down_log[j]);
							cross_gt.push_back(temp_cross);
							temp_cross = abs(re_up_log[j] - re_down_log[j]);
							cross_re.push_back(temp_cross);
						}
						//记录得到的结果
						ofstream logg(base_result_root + add[il]);
						logg << endl;
						logg << "gt_up ";
						for (int j = 0; j < gt_up_log.size(); j++) {
							logg << gt_up_log[j] << " ";
						}
						logg << endl;
						logg << "gt_down ";
						for (int j = 0; j < gt_down_log.size(); j++) {
							logg << gt_down_log[j] << " ";
						}
						logg << endl;
						logg << "re_up ";
						for (int j = 0; j < re_up_log.size(); j++) {
							logg << re_up_log[j] << " ";
						}
						logg << endl;
						logg << "re_down ";
						for (int j = 0; j < re_down_log.size(); j++) {
							logg << re_down_log[j] << " ";
						}
						logg.close();
						//记录竖向准确度
						ofstream logg2(base_result_root1 + add[il]);
						logg2 << endl;
						logg2 << "up ";
						for (int j = 0; j < gt_up_log.size(); j++) {
							logg2 << (1 - abs(gt_up_log[j] - re_up_log[j]) / distance) * 100 << " ";
						}
						logg2 << endl;
						logg2 << "down ";
						for (int j = 0; j < gt_down_log.size(); j++) {
							logg2 << (1 - abs(gt_down_log[j] - re_down_log[j]) / distance) * 100 << " ";
						}
						logg2.close();
						//记录竖向跨度
						ofstream cross_file(base_result_root2 + add[il]);
						cross_file << endl << "cross_gt ";
						for (int j = 0; j < cross_gt.size(); j++) {
							cross_file << cross_gt[j] << " ";
						}
						cross_file << endl << "cross_re ";
						for (int j = 0; j < cross_re.size(); j++) {
							cross_file << cross_re[j] << " ";
						}
						cross_file << endl << "cross_re ";
						for (int j = 0; j < cross_re.size(); j++) {
							cross_file << (1 - abs(cross_gt[j] - cross_re[j]) / cross_gt[j]) * 100 << " ";
						}
						cross_file.close();
						//计算竖向密度分布
						ofstream density_file(base_result_root3 + add[il]);
						density_file << endl << "density_gt ";
						for (int j = 0; j < gt_point.size(); j++) {
							density_file << setprecision(4) << gt_point[j] * 100 << " ";
						}
						density_file << endl << "density_re ";
						for (int j = 0; j < re_point.size(); j++) {
							density_file << setprecision(4) << re_point[j] * 100 << " ";
						}
						density_file.close();
						//getchar();
						//在matlab上绘制结果图，运行3*4组结果进行对比
					}
				}
			}
		}
}
  return EXIT_SUCCESS;
}
