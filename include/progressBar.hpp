#pragma once


namespace tools{
    void ProgessBar (double percentage, int type=0, bool flush=true){
        //Calculate percentage
        switch (type) {
            case 1:// Percent
                printf("%5.1f%%", percentage*100);
                if(flush) fflush (stdout);
                break;
            default: // bar & percent
            {
                static std::string pbstr = "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
                static int pbwidh = 60;
                int val = (int) (percentage * 100);
                int lpad = (int) (percentage * pbwidh);
                int rpad = pbwidh - lpad;
                printf ("\r[%.*s%*s] %3d%% ", lpad, pbstr.c_str(), rpad, "", val);
                if(flush) fflush (stdout);
            }
                break;
        }
    }
}
