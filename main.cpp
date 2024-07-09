#include <iostream>
#include <fstream>
#define  _WIN32_WINNT  0x0600
#include <Windows.h>
#define debug 1
#include <dirent.h>

class SSC
{
    std::string dict;
    std::ofstream errors_logs;
public:

    SSC()
    {
        std::ifstream dict_data(myreplace(get_exe_path(),"special_symbol_cleaner.exe","")+"dictionary.txt");
        std::getline(dict_data,dict);
        dict_data.close();
        //std::cout<<dict<<std::endl;

        errors_logs.open("errors_logs.txt");
    }

    ~SSC()
    {
        dict.clear();
        errors_logs.close();
    }

    std::string myreplace(std::string s, std::string from, std::string to)
    {
        size_t pos=s.find(from);
        while(pos!=std::string::npos)
        {
            s.replace(pos,from.size(),to);
            pos+=to.size();
            pos=s.find(from,pos);
        }
        return s;
    }

    std::string rename_dir(std::string name)
    {
        std::string res_name="";
        for(int i=0;i<name.size();i++)
            if(dict.find(name[i])!=std::string::npos)
                {
                    res_name+=name[i];
                    std::cout<<name[i]<<" "<<dict[dict.find(name[i])]<<std::endl;
                }
            else res_name+='-';

        return res_name;
    }


    void check_path(std::string &path)
    {
        path=myreplace(path,"\\","/");
        if(path[path.size()-1]!='/')
        {
            if(debug)std::cout<<"warning: the path must end in \"/\""<<std::endl;
            path+="/";
        }
    }
    std::string get_exe_path()
    {
        char result[ MAX_PATH ];
        return std::string( result, GetModuleFileName( NULL, result, MAX_PATH ) );
    }
    void check_registry_errors(long errorc)
    {
        if(errorc!=ERROR_SUCCESS)
        {
            if(debug)std::cout<<"error in registry: "<<errorc<<std::endl;
            return;
        }
    }

    void setup()
    {
        HKEY hKey;
        std::string data;
        std::string exe_path=get_exe_path();



        check_registry_errors (RegCreateKeyExW (HKEY_CLASSES_ROOT,L"Folder\\shell\\renameWithSSC",0,L"",REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&hKey,NULL));
        if(hKey)
        {
            data="Call SSC";
            check_registry_errors (RegSetValueEx (hKey, "MUIVerb", 0, REG_SZ, (LPBYTE)data.c_str(), strlen(data.c_str())));
            RegCloseKey(hKey);
        }

        check_registry_errors (RegCreateKeyExW (HKEY_CLASSES_ROOT,L"Folder\\shell\\renameWithSSC\\command",0,L"",REG_OPTION_NON_VOLATILE,KEY_SET_VALUE,NULL,&hKey,NULL));
        if(hKey)
        {

            data="cmd /c \"cd /d %1 &&echo %1| "+exe_path+" \"";
            check_registry_errors (RegSetValueEx (hKey, "", 0, REG_SZ, (LPBYTE)data.c_str(), strlen(data.c_str())));
            RegCloseKey(hKey);
        }


    }

    void delete_from_registry()
    {
        check_registry_errors (RegDeleteTreeW (HKEY_CLASSES_ROOT,L"Folder\\shell\\renameWithSSC"));
    }



    void read_folder(std::string start_path, std::string filename="")
    {


        DIR *dpdf;
        struct dirent *epdf;

        dpdf = opendir((start_path+filename).c_str());
        std::cout<<(start_path+filename)<<std::endl;

        if (dpdf != NULL)
        {
            for (int i=0; epdf = readdir(dpdf); i++)
            {
                if(i>=2)
                {
                    if ( rename((start_path+filename+epdf->d_name).c_str(),(start_path+filename+rename_dir(epdf->d_name)).c_str() ) == 0 &&debug )
                        std::cout << epdf->d_name<<std::endl;
                    else if(debug)
                    {
                        std::cout <<"error: "<< epdf->d_name<<std::endl;
                        errors_logs<<"error: "<<start_path+filename+epdf->d_name<<std::endl;
                    }
                    read_folder(start_path,filename+rename_dir(epdf->d_name)+"/");
                }

            }
        }
        //else std::cout<<"directory is not opened"<<std::endl;
        closedir(dpdf);
    }
};
int main()
{
    setlocale(1251,NULL);
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    SSC ssc;
    std::string path;
    bool run=true;

    while(run)
    {
        getline(std::cin,path);
        run=(path=="setup"||path=="delete"||path=="help");
        if(path=="setup")
            ssc.setup();
        else if(path=="delete")
            ssc.delete_from_registry();
        else if(path=="help")
        {
            const char* msg =
                "Usage: myarchiver.exe [args]: \n"
                "|\thelp   THE ALL MIGHTY HELP\n"
                "|\tsetup   Setup the program to the registry\n"
                "|\tdelete Uninstall the program from the registry\n"
                "|\tpath Replaces all characters that are not in the file \"dictionary.txt \" on \"-\"\n"
                ;
            std::cout<<msg<<std::endl;
        }
        else
        {

            ssc.check_path(path);
            ssc.read_folder(path);
        }
        std::cout<<"done"<<std::endl<<std::endl;
    }

    return 0;
}
