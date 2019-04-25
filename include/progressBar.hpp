#pragma once


namespace tools{
    void ProgessBar (const int& current, const int& total, int type, bool flush=true){
        //Calculate percentage
        float percentage = (float)(current)/total*10;
        switch (type) {
            case 1:// Percent
                printf("%5.1f%%", percentage*10);
                if(flush) fflush (stdout);
                break;
            case 2:// Number
                printf("[%d/%d]", current, total);
                if(flush) fflush (stdout);
                break;
            case 3:// Percent & Number
                printf("%5.1f%%[%d/%d]", percentage*10, current, total);
                if(flush) fflush (stdout);
                break;
            default: // bar & percent
            {
                static std::string pbstr = "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
                static int pbwidh = 60;
                int val = (int) (percentage * 100);
                int lpad = (int) (percentage * pbwidh);
                int rpad = pbwidh - lpad;
                printf ("\r%3d%% [%.*s%*s]", val, lpad, pbstr.c_str(), rpad, "");
                if(flush) fflush (stdout);
            }
                break;
        }
    }
}
