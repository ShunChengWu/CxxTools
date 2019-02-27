//
//  Tools_plot.hpp
//  pointnet_cpp
//
//  Created by Shun-Cheng Wu on 18/10/2017.
//
//

#ifndef Tool_plot_hpp
#define Tool_plot_hpp

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#include <fstream> //Load & Save file


namespace tools {
    void PCA_plot (cv::Mat& mat, std::vector<int> label, const int& num_category, const int& dim, const int& test_run, std::string out_img_name = "Pointnet_PCA_2d.jpg", std::string out_3d_name = "Pointnet_PCA_3d.txt");
    void Evaluation_plot (std::vector<int> label_p, std::vector<int> label_t, const int& class_num);
    
} //End of namespace Tools






#endif /* Tool_plot_hpp */
