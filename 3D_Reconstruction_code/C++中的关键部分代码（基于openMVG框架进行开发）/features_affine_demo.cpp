// This file is base on the framework of OpenMVG, an Open Multiple View Geometry C++ library.
//Ω¯––∑¬…‰µ„∆•≈‰
#include "openMVG/features/feature.hpp"
#include "openMVG/features/mser/mser.hpp"
#include "openMVG/features/mser/mser_region.hpp"
#include "openMVG/features/PEFR/PEFR.hpp"
#include "openMVG/image/image_io.hpp"
#include "openMVG/image/image_drawing.hpp"
#include "openMVG/image/image_resampling.hpp"
#include "openMVG/image/sample.hpp"

#include "third_party/stlplus3/filesystemSimplified/file_system.hpp"
#include "third_party/cmdLine/cmdLine.h"


#include <unsupported/Eigen/MatrixFunctions>

#include <iostream>
#include <string>
#include <fstream>
#include <regex>
#include <cmath>

using namespace openMVG;
using namespace openMVG::image;
using namespace openMVG::features;
using namespace std;


template <typename Image>
void NormalizePatch
(
  const Image &src_img,
  const AffinePointFeature &feat ,
  const int patch_size ,
  Image &out_patch

)
{
  // Mapping function
  Eigen::Matrix<double,2,2> A;
  A << feat.a(), feat.b(),
       feat.b(), feat.c();

  // Inverse square root
  A = A.pow( -0.5 );

  const float sc = 2.f * 3.f / static_cast<float>( patch_size );
  A = A * sc;

  const float half_width = static_cast<float>( patch_size ) / 2.f;

  // Compute sampling grid
  std::vector< std::pair<float,float> > sampling_grid;
  sampling_grid.reserve(patch_size*patch_size);
  for (int i = 0; i < patch_size; ++i )
  {
    for (int j = 0; j < patch_size; ++j )
    {
      // Apply transformation relative to the center of the patch (assume origin at 0,0 then map to (x,y) )
      Vec2 pos;
      pos << static_cast<float>( j ) - half_width, static_cast<float>( i ) - half_width;
      // Map (ie: ellipse transform)
      const Vec2 affineAdapted = A * pos;

      sampling_grid.emplace_back( affineAdapted(1) + feat.y() , affineAdapted(0) + feat.x() );
    }
  }

  Sampler2d< SamplerLinear > sampler;

  // Sample input image to generate patch
  GenericRessample(
    src_img , sampling_grid ,
    patch_size , patch_size ,
    sampler ,
    out_patch );
}

void Extract_MSER
(
  const Image<unsigned char> & img,
  std::vector<features::AffinePointFeature> & feats_dark,
  std::vector<features::AffinePointFeature> & feats_bright
)
{
  using namespace openMVG::features::MSER;

  //-- Extract Bight MSER
  {
    // Inverted image
    Image<unsigned char> image4( 255 - img.array() );
    std::vector< MSERRegion > regs;
    MSERExtractor extr4( 2 , 0.0005 , 0.1 , 0.5 , 0.5 , MSERExtractor::MSER_4_CONNECTIVITY );
    extr4.Extract( image4 , regs );
    for (size_t i = 0; i < regs.size(); ++i )
    {
      double a, b, c;
      regs[i].FitEllipse( a, b, c );
      double x, y;
      regs[i].FitEllipse( x, y );
      feats_bright.emplace_back(x, y, a, b, c);
    }
  }

  //-- Extract Dark MSER
  {
    std::vector< MSERRegion > regs;
    MSERExtractor extr8( 2 , 0.0005 , 0.1 , 0.5 , 0.5 , MSERExtractor::MSER_8_CONNECTIVITY );
    extr8.Extract( img , regs );
    for (size_t i = 0; i < regs.size(); ++i )
    {
      double a, b, c;
      regs[i].FitEllipse( a, b, c );
      double x, y;
      regs[i].FitEllipse( x, y );
      feats_dark.emplace_back(x, y, a, b, c);
    }
  }
}

void Extract_PEFR
(
  const Image<unsigned char> & img,
  std::vector<features::AffinePointFeature> & feats_dark,
  std::vector<features::AffinePointFeature> & feats_bright
)
{
  PEFR::Extract_PEFR(img, feats_bright, std::less<uint8_t> (), 30);
  PEFR::Extract_PEFR(img, feats_dark, std::greater<uint8_t> (), 30);
}

void read_imglist(const std::string file_root, vector<std::string> &imglist) {
    std::string re;
	ifstream inf(file_root + "imglist.txt");
	while (!inf.eof()) {
		inf >> re;
		imglist.push_back(re);
		//std::cout << re << std::endl;
	}
	imglist.pop_back();
}

vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}

int main(int argc, char **argv)
{
	std::string sAffine_Detector_Method = "MSER";

	argv[1] = "-d";
	argv[2] = "MSER";
	argv[3] = "";
	argc = 4;

	CmdLine cmd;
	cmd.add(make_switch('P', "PATCH"));
	cmd.add(make_option('d', sAffine_Detector_Method, "detector"));

	std::cout
		<< "PEFR Demo:\n"
		<< " Show detected Affine regions as ellipses,\n"
		<< " -[P] in the command line exports square normalized patches for each ellipses.\n"
		<< " -[d|detector] PEFR|MSER Detect PEFR or MSER affine regions."
		<< std::endl;

	try {
		cmd.process(argc, argv);
	}
	catch (const std::string& s) {
		std::cerr << s << std::endl;
		return EXIT_FAILURE;
	}
	//const std::string sInputDir =
	//  stlplus::folder_up(string(THIS_SOURCE_DIR)) + "/imageData/SceauxCastle/";
	//const std::string jpg_filename = sInputDir + "100_7101.jpg";
	int step;
	step = 3;
	
	//MSER result and PEFR result
	if (step == 1) {
		const std::string sInputDir = "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/";
		vector<std::string> imglist;
		read_imglist(sInputDir, imglist);
		std::string jpg_filename = "";

		//read image in list and write in image
		Image<unsigned char> image;
		Image<RGBColor> imagec;
		vector <std::string>::const_iterator t;
		int count_i = 1, count_all = imglist.size();
		for (t = imglist.begin(); t != imglist.end(); t++)
		{
			//read image and count
			std::cout << count_i << "/" << count_all << std::endl;
			jpg_filename = sInputDir + *t;
			ReadImage(jpg_filename.c_str(), &image);
			ReadImage(jpg_filename.c_str(), &imagec);

			//get type of dataset and filename
			vector<string> sma;
			sma = split(*t, "/");

			//get MSER feature
			std::vector<features::AffinePointFeature>feats_dark, feats_bright;
			Extract_MSER(image, feats_dark, feats_bright);

			//draw mser bright regions
			std::cout << "MSER Bright Region : " << feats_bright.size() << std::endl;
			Image<RGBColor> Icpy(imagec);
			for (size_t i = 0; i < feats_bright.size(); ++i) {
				const AffinePointFeature & fp = feats_bright[i];
				DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), RGBColor(255, 255, 0), &Icpy, fp.orientation());
				//DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255 , &Icpy, fp.orientation());
			}

			//output mser bright regions image
			std::ostringstream os;
			os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/result/" << sma[0] << "/MSER_BRIGHT_" << sma[1];
			WriteImage(os.str().c_str(), Icpy);

			//draw mser dark regions image
			std::cout << "MSER Dark Region :" << feats_dark.size() << std::endl;
			Icpy = imagec;
			for (size_t i = 0; i < feats_dark.size(); ++i) {
				const AffinePointFeature & fp = feats_dark[i];
				DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), RGBColor(255,255,0), &Icpy, fp.orientation());
				//DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
			}
			os.str("");
			os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/result/" << sma[0] << "/MSER_DARK_" << sma[1];
			WriteImage(os.str().c_str(), Icpy);

			//get PEFR region
			std::vector<features::AffinePointFeature>tfeats_dark, tfeats_bright;
			Extract_PEFR(image, tfeats_dark, tfeats_bright);

			//draw and output PEFR bright region
			std::cout << "PEFR Bright Region : " << tfeats_bright.size() << std::endl;
			Icpy = imagec;
			for (size_t i = 0; i < tfeats_bright.size(); ++i) {
				const AffinePointFeature & fp = tfeats_bright[i];
				DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), RGBColor(255,255,0), &Icpy, fp.orientation());
				//DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
			}
			os.str("");
			os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/result/" << sma[0] << "/PEFR_BRIGHT_" << sma[1];
			WriteImage(os.str().c_str(), Icpy);

			//draw and output PEFR dark region
			std::cout << "PEFR Dark Region :" << tfeats_dark.size() << std::endl;
			Icpy = imagec;
			for (size_t i = 0; i < tfeats_dark.size(); ++i) {
				const AffinePointFeature & fp = tfeats_dark[i];
				DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), RGBColor(255,255,0), &Icpy, fp.orientation());
				//DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
			}
			os.str("");
			os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/result/" << sma[0] << "/PEFR" << sma[1];
			WriteImage(os.str().c_str(), Icpy);

			count_i++;
		}
		return EXIT_SUCCESS;
	}

	//background eliminate
	if (step == 2) {
		const std::string sInputDir = "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/step2/";
		vector<std::string> imglist;
		read_imglist(sInputDir, imglist);
		std::string jpg_filename = "";

		//read image in list and write in image
		Image<unsigned char> image;
		Image<RGBColor> imagec;
		vector <std::string>::const_iterator t;
		int count_i = 1, count_all = imglist.size();
		for (t = imglist.begin(); t != imglist.end(); ++t)
		{
			//read image and count
			std::cout << count_i << "/" << count_all << std::endl;
			jpg_filename = sInputDir + *t;
			ReadImage(jpg_filename.c_str(), &image);
			ReadImage(jpg_filename.c_str(), &imagec);

			//get type of dataset and filename
			vector<string> sma;
			sma = split(*t, "/");

			Image<RGBColor> Icpy(imagec);
			std::ostringstream os;

			//get PEFR region
			std::vector<features::AffinePointFeature>tfeats_dark, tfeats_bright;
			Extract_PEFR(image, tfeats_dark, tfeats_bright);

			//draw and output PEFR bright region
			std::cout << "PEFR Bright Region : " << tfeats_bright.size() << std::endl;
			for (size_t i = 0; i < tfeats_bright.size(); ++i) {
				const AffinePointFeature & fp = tfeats_bright[i];
				int laround = 0, saround = 0;
				//if L point
				if (fp.l1() < 22 && fp.l2() < 22 && fp.l1() + fp.l2() < 30) {
					for (size_t j = 0; j < tfeats_bright.size(); ++j) {
						const AffinePointFeature & cfp = tfeats_bright[j];
						if (cfp.l1() >= 22 || cfp.l2() >= 22 || cfp.l1() + cfp.l2() >= 30) {
							//if around L point
							if (abs(cfp.x() - fp.x()) + abs(cfp.y() - fp.y()) <= (cfp.l1()+cfp.l2()))
								laround++;
						}
						else {
							//if around S point
							if (abs(cfp.x() - fp.x()) + abs(cfp.y() - fp.y()) <= 10)
								saround++;
						}
						if(laround > 1 && saround <= 5)
							DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), RGBColor(255,255,0), &Icpy, fp.orientation());
					}
				}
				else {
					DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), RGBColor(255,255,0), &Icpy, fp.orientation());
				}
			}
			os.str("");
			os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/step2/result/PEFR_BRIGHT_" << sma[1];
			WriteImage(os.str().c_str(), Icpy);

			////draw and output PEFR dark region
			//std::cout << "PEFR Dark Region :" << tfeats_dark.size() << std::endl;
			//Icpy = image;
			//for (size_t i = 0; i < tfeats_dark.size(); ++i) {
			//	const AffinePointFeature & fp = tfeats_dark[i];
			//	DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
			//}
			//os.str("");
			//os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/step2/result/PEFR_DARK_" << sma[1];
			//WriteImage(os.str().c_str(), Icpy);

			count_i++;
		}		
	}

	//region output
	if (step == 3) {
		const std::string sInputDir = "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/wraped_image/";
		vector<std::string> imglist;
		read_imglist(sInputDir, imglist);
		std::string jpg_filename = "";

		//("C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/wraped_image/out1.txt");

		//read image in list 
		Image<unsigned char> image;
		//Image<RGBColor> imagec;
		vector <std::string>::const_iterator t;
		int count_i = 1, count_all = imglist.size(), count_r = 1;
		for (t = imglist.begin(); t != imglist.end(); ++t)
		{
			//read image and count
			std::cout << count_i << "/" << count_all << std::endl;
			jpg_filename = sInputDir + *t;
			ReadImage(jpg_filename.c_str(), &image);
			//ReadImage(jpg_filename.c_str(), &imagec);
			int count_l = 1;

			//get type of dataset and filename
			vector<string> sma;
			sma = split(*t, "/");

			Image<unsigned char> Icpy(image);
			//Image<RGBColor> Icpy(imagec);
			std::ostringstream os;
			std::ofstream file;
			string file_ss = "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/wraped_image/out" + to_string(count_i) + ".txt";

			//get precise region
			std::vector<features::AffinePointFeature>tfeats_dark, tfeats_bright;
			Extract_PEFR(image, tfeats_dark, tfeats_bright);

			//draw and output PEFR bright region
			std::cout << "Bright Region : " << tfeats_bright.size() << std::endl;
			int if_foreground = 0;
			for (size_t i = 0; i < tfeats_bright.size(); ++i) {
				const AffinePointFeature & fp = tfeats_bright[i];
				int laround = 0, saround = 0;
				//if L point
				if (fp.l1() < 22 && fp.l2() < 22 && fp.l1() + fp.l2() < 30) {
					for (size_t j = 0; j < tfeats_bright.size(); ++j) {
						const AffinePointFeature & cfp = tfeats_bright[j];
						if (cfp.l1() >= 22 || cfp.l2() >= 22 || cfp.l1() + cfp.l2() >= 30) {
							//if around L point
							if (abs(cfp.x() - fp.x()) + abs(cfp.y() - fp.y()) <= (cfp.l1() + cfp.l2()))
								laround++;
						}
						else {
							//if around S point
							if (abs(cfp.x() - fp.x()) + abs(cfp.y() - fp.y()) <= 10)
								saround++;
						}
						if (laround > 1 && saround <= 5) {
							DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
							if_foreground = 1;
						}
					}
				}
				else {
					DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
					if_foreground = 1;
				}
				if (if_foreground == 1) {
					file.open(file_ss, std::ios::out | std::ios::app);
					if (file.is_open()) {
						file << count_l++ << "|(" << fp.x() << "," << fp.y() << ")|(" << fp.l1() << "," << fp.l2() << ")|" << fp.orientation() - 1.57 << "\n";
						file.close();
					}
					if_foreground = 0;
				}
			}
			os.str("");
			os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/wraped_image/result/result_b_" << sma[1];
			openMVG::image::WriteImage(os.str().c_str(), Icpy);

			std::cout << "Dark Region : " << tfeats_dark.size() << std::endl;
			if_foreground = 0;
			for (size_t i = 0; i < tfeats_dark.size(); ++i) {
				const AffinePointFeature & fp = tfeats_dark[i];
				int laround = 0, saround = 0;
				//if L point
				if (fp.l1() < 22 && fp.l2() < 22 && fp.l1() + fp.l2() < 30) {
					for (size_t j = 0; j < tfeats_dark.size(); ++j) {
						const AffinePointFeature & cfp = tfeats_dark[j];
						if (cfp.l1() >= 22 || cfp.l2() >= 22 || cfp.l1() + cfp.l2() >= 30) {
							//if around L point
							if (abs(cfp.x() - fp.x()) + abs(cfp.y() - fp.y()) <= (cfp.l1() + cfp.l2()))
								laround++;
						}
						else {
							//if around S point
							if (abs(cfp.x() - fp.x()) + abs(cfp.y() - fp.y()) <= 10)
								saround++;
						}
						if (laround > 1 && saround <= 5) {
							DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
							if_foreground = 1;
						}
					}
				}
				else {
					DrawEllipse(fp.x(), fp.y(), fp.l1(), fp.l2(), 255, &Icpy, fp.orientation());
					if_foreground = 1;
				}
				if (if_foreground = 1) {
					file.open(file_ss, std::ios::out | std::ios::app);
					if (file.is_open()) {
						file << count_l++ << "|(" << fp.x() << "," << fp.y() << ")|(" << fp.l1() << "," << fp.l2() << ")|" << fp.orientation() - 1.57 << "\n";
						file.close();
					}
					if_foreground = 0;
				}
			}
			os.str("");
			os << "C:/Users/Fbj/Desktop/vgg_oxford_feat_eval/wraped_image/result/result_d_" << sma[1];
			openMVG::image::WriteImage(os.str().c_str(), Icpy);

			count_i++;
		}
	}
	return EXIT_SUCCESS;
}
