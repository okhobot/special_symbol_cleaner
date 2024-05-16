#include <iostream>
#define  _WIN32_WINNT  0x0600
#include <Windows.h>
#define debug 1
#include <dirent.h>

class SSC
{
public:
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
                    read_folder(start_path,filename+epdf->d_name+"/");
                    if ( rename((start_path+filename+epdf->d_name).c_str(),(start_path+filename+myreplace(epdf->d_name,"—","-")).c_str() ) == 0 &&debug )
                        std::cout << epdf->d_name<<std::endl;
                    else if(debug)std::cout <<"error: "<< epdf->d_name<<std::endl;
                }

            }
        }
        else std::cout<<"directory is not opened"<<std::endl;
        closedir(dpdf);
    }
};
int main()
{
    setlocale(1251,"Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);


    SSC ssc;
    std::string path;
    getline(std::cin,path);
    if(path=="setup")
        ssc.setup();
    else if(path=="delete")
        ssc.delete_from_registry();
    else
    {
        ssc.check_path(path);
        ssc.read_folder(path);
    }

    return 0;
}
