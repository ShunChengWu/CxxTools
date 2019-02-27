//
//  tool_plot.cpp
//  pointnet_cpp
//
//  Created by Shun-Cheng Wu on 18/10/2017.
//
//

#include "utilities/opencv/Tool_plot.hpp"



namespace tools{
    void PCA_plot (cv::Mat& mat, std::vector<int> label, const int& num_category, const int& dim, const int& test_run, std::string out_img_name, std::string out_3d_name){
        cv::PCA pca_analysis(mat, cv::noArray(), CV_PCA_DATA_AS_ROW, dim);
        pca_analysis.project(mat,mat);
        cv::normalize(mat, mat, 0, 10, cv::NORM_MINMAX, -1);
        
        cv::Mat imgshow (320, 320, CV_8UC3, cvScalarAll(0));
        cv::RNG rng(12345);
        std::vector<cv::Scalar> cate_color(num_category);
        for (int i=0;i<num_category;++i){
            cate_color[i]=cv::Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
        }
        
        for (int i = 0; i < test_run; ++i){
            //            std::cout << mat.at<float>(i,0)*5+110 <<" "<< mat.at<float>(i,1)*5+110 << std::endl;
            cv::Point2f centre = cv::Point2f(mat.at<float>(i,0)*5+110, mat.at<float>(i,1)*5+110);
            cv::circle(imgshow, centre, 1, cate_color[label[i]], -1);
        }
        cv::imwrite("Pointnet_PCA_2d.jpg", imgshow);
        cv::imshow("PCA_Plot", imgshow);
        cv::waitKey(0);
        
        std::ofstream output_file("Pointnet_PCA_3d.txt");
        for (int i = 0; i < test_run; ++i){
            output_file << mat.at<float>(i,0) << " " << mat.at<float>(i,1) << " " << mat.at<float>(i,2) <<
            " " << label[i]<<"\n";
        }
    }
    
    void Evaluation_plot (std::vector<int> label_p, std::vector<int> label_t, const int& class_num){
        cv::Mat EvaMat (720, 720, CV_32FC3, cvScalarAll(0));
        float block_size = 720/(class_num+1);
        
        //Evaluate data and scale in 255
        for (int i=0;i<label_p.size();++i) {
            //            std::cout<<(label_p[i]+1)<<" "<< (label_p[i]+2)<<" "<<(label_t[i]+1)<<" "<<(label_t[i]+2)<<std::endl;
            //            std::cout<<(label_p[i]+1)*block_size<<" "<< (label_p[i]+2)*block_size<<" "<<(label_t[i]+1)*block_size<<" "<<(label_t[i]+2)*block_size<<std::endl;
            cv::Mat temp = EvaMat.rowRange((label_p[i]+1)*block_size, (label_p[i]+2)*block_size).colRange((label_t[i]+1)*block_size, (label_t[i]+2)*block_size);
            cv::add(temp, 1, temp);
        }
        double max, min;
        cv::minMaxLoc(EvaMat, &min, &max);
        EvaMat /=max;
        EvaMat *= 255;
        
        //Put text
        float font_scale = 1/float(class_num)*10;
        cv::Size textsize = cv::getTextSize("True", cv::FONT_HERSHEY_SIMPLEX, font_scale*0.9, 1, 0);
        float font_cor_w=(block_size-textsize.width)/2;
        cv::putText(EvaMat, "True", cv::Point(font_cor_w, (block_size-textsize.height)/2), cv::FONT_HERSHEY_SIMPLEX, font_scale, cvScalarAll(255));
        cv::putText(EvaMat, "Pred", cv::Point(font_cor_w, (block_size+textsize.height)/2), cv::FONT_HERSHEY_SIMPLEX, font_scale, cvScalarAll(255));
        //Put number
        for(int i = 0; i < class_num; ++i){
            cv::Size textsize = cv::getTextSize(std::to_string(i), cv::FONT_HERSHEY_SIMPLEX, font_scale, 1, 0);
            float font_cor_w=(block_size-textsize.width)/2;
            float font_cor_h=(block_size+textsize.height)/2;
            float font_block=(i+1)*block_size;
            
            cv::putText(EvaMat, std::to_string(i), cv::Point(font_block+font_cor_w, font_cor_h), cv::FONT_HERSHEY_SIMPLEX, font_scale, cvScalarAll(255));
            cv::putText(EvaMat, std::to_string(i), cv::Point(font_cor_w, font_block+font_cor_h), cv::FONT_HERSHEY_SIMPLEX, font_scale, cvScalarAll(255));
        }
        
        EvaMat.convertTo(EvaMat, CV_8UC1);
        
        cv::imshow("Evaluation matrix", EvaMat);
        cv::waitKey(0);
    }
}
