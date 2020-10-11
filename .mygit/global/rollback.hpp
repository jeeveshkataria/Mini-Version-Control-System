namespace gitrollback
{
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <dirent.h>
#include <sys/types.h>
#include <fstream>


using namespace std;
#define INDEX_FILE "index.txt"
#define VERSION_FILE "version_no.txt"
string current_path_global;
char cwd[PATH_MAX];
string mygit_path;
string vcpath;
string vc;
string indexpath;
vector<string> filelist;
map<string, string> filemap;
using namespace std;

map<string, string> get_map_from(string path)
{ cout<<"entered getmap func"<<endl;
    map<string, string> toret;
    fstream f(path, std::ios_base::in);
    if (f.is_open())
    {
        string line;
        while (getline(f, line))
        {
            stringstream ss(line);
            string filename = "";
            string sha = "";
            ss >> filename;
            ss >> sha;
            toret[filename] = sha;
        }
    }
    else
    {
        perror("Unable to open file information get_map_from function");
        exit(1);
    }
    return toret;

}


vector<string> get_files_from(string path)
{
    vector<string> toret;
    struct dirent *entry;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL)
    {
        cout << "Error in opening Directory";
        exit(EXIT_FAILURE);
    }
    while ((entry = readdir(dir)) != NULL)
    {
        //cout << entry->d_name << endl;
        string fname = entry->d_name;
        if (fname == "." || fname == ".." || fname == ".mygit" || fname == "a.out" || fname == ".vscode" || fname=="mygit")
        {
            continue;
        }
        else
        {
            toret.push_back(fname);
        }
    }
    closedir(dir);
    return toret;
}
//2 -> vno >= currVer, 1 -> vno<currver and succ terminated

int roll_back(int curr_ver)
{
    int to_ver_no = curr_ver-1;
    string lastverno = to_string(to_ver_no);
    mygit_path = "";
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        mygit_path = cwd;
        mygit_path += "/.mygit/";
    }
    else
    {
        perror("Unable to get current working directory!\n");
        exit(1);
    }
    vcpath = mygit_path + VERSION_FILE;
   
    fstream vcfile(vcpath, std::ios_base::in);
    vcfile >> vc;
    indexpath = mygit_path + vc + "/" + INDEX_FILE;
    
    

    string currverpath = mygit_path+vc;
    
    DIR* folder = opendir(currverpath.c_str());
    struct dirent   *next_file;
    char filepath[PATH_MAX];
     while ( (next_file = readdir(folder)) != NULL )
    {
        // build the path for each file in the folder
        sprintf(filepath, "%s/%s", currverpath.c_str(), next_file->d_name);
        remove(filepath);
    }
    closedir(folder);
  string lastverpath = mygit_path+lastverno;
  string cmd2 = "cp -a "+lastverpath+"/. "+currverpath;
  system(cmd2.c_str());

  
  filemap = get_map_from(indexpath);
 current_path_global = mygit_path+"global/";

  for (auto i=filemap.begin();i!=filemap.end();i++){
      string ifirst = i->first;
      string sourcefilepath = current_path_global+ifirst;
      string destfilepath = currverpath+"/"+ifirst;

    ifstream ifile(sourcefilepath, ios::in); 
    ofstream ofile(destfilepath, ios::out); 
     if (!ifile.is_open()) {
        cout << "file not found"; 
    } 
    else { 
        ofile << ifile.rdbuf(); 
    } 
  }

  //run patch command for all patch fikes

for (auto i=filemap.begin();i!=filemap.end();i++){
      string ifirst = i->first;
      string sourcefilepath = currverpath+"/"+ifirst;
      string patchfilepath = currverpath+"/"+ifirst+".patch";

   string cmd = "patch "+sourcefilepath + " " + patchfilepath;
   system(cmd.c_str()); 
  }
  
for (auto i=filemap.begin();i!=filemap.end();i++){
      string ifirst = i->first;
      string patchfilepath = currverpath+"/"+ifirst+".patch";
 remove(patchfilepath.c_str());
  }


  folder = opendir(currverpath.c_str());
  char fpathlocal[PATH_MAX];
     while ( (next_file = readdir(folder)) != NULL )
    {
        // build the path for each file in the folder
        sprintf(fpathlocal, "%s/%s", cwd, next_file->d_name);
        if(fpathlocal=="a.out") continue;
        
        remove(fpathlocal);
    }
    cout<<"yaha ho gya!!"<<endl;
    closedir(folder);

for (auto i=filemap.begin();i!=filemap.end();i++){
      string ifirst = i->first;
      if(ifirst=="index.txt")continue;
      string sourcefilepath = currverpath+"/"+ifirst;
      string cwds =cwd;
      string destfilepath = cwds+"/"+ifirst;

    ifstream ifile(sourcefilepath, ios::in); 
    ofstream ofile(destfilepath, ios::out); 
     if (!ifile.is_open()) {
        cout << "file not found"; 
    } 
    else { 
        ofile << ifile.rdbuf(); 
    } 
  }
  
    return 1;
}

}
}



