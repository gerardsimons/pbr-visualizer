/* loadppm.cpp */
#include "loadppm.h"



PPMImage::PPMImage(const char *filename)
{
    char buff[16];

	std::ifstream fp(filename, std::ios::in|std::ios::binary);
    int maxval,lig_comm=0;

    if (!fp.is_open()){
	std::cerr <<"Unable to open file " << filename << std::endl;
	throw 0;
    }

    fp.read(buff, 3);
    if (fp.bad()){
      std::cerr <<"Unable to open file " << filename << std::endl;
      throw 0;
    }
    
    if (buff[0] != 'P' || buff[1] != '6'){
	std::cerr << "Invalid image format (must be `P6')\n" ;
	throw 0;
    }

    if (buff[2] != 0x0a)
      fp.seekg(-1,std::ios::cur);

    while (fp.read(buff, 1), buff[0] == '#'){
      lig_comm++;
      while (fp.read(buff, 1), buff[0] != '\n');
    }

    fp.seekg(-1,std::ios::cur);

    fp >> sizeX;
    fp >> sizeY;
    if (fp.bad()){
      std::cerr << "Error loading image " << filename << std::endl ;
      throw 0;
    }

    fp >> maxval;
    if (fp.bad()){
      std::cerr << "Error loading image " << filename << std::endl ;
      throw 0;
    }

    while (fp.read(buff, 1), buff[0] != '\n')
      ;

    data = new  GLubyte[3 * sizeX * sizeY];
    if (!data)
    {
      std::cerr <<  "Unable to allocate memory\n";
      throw 0;
    }

    // Chargement de l'image directement
    fp.read((char*)data, 3 * sizeX*sizeY);
    if (fp.bad()) {
      std::cerr <<  "Error loading image " << filename << std::endl;
      throw 0;
    }
    
    fp.close();
}

}