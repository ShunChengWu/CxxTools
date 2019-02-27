//
//  Tools.cpp
//  pointnet_cpp
//
//  Created by Shun-Cheng Wu on 18/10/2017.
//
//

#include <Tools.hpp>

namespace tools{
    void ProgessBar (const int& current, const int& total, int type){
        //Calculate percentage
        float percent = (float)(current)/total*10;
        switch (type) {
            case 1:// Percent
                printf("%5.1f%%", percent*10);
                break;
            case 2:// Number
                printf("[%d/%d]", current, total);
                break;
            case 3:// Percent & Number
                printf("%5.1f%%[%d/%d]", percent*10, current, total);
                break;
            default:// Percent & Bar:
                printf("%5.1f%%[", percent*10);
                for (int i=0;i<10;++i)
                    i<percent? std::cout<<"=":std::cout<<"-";
                std::cout<<"] ";
                break;
        }
    }
}
